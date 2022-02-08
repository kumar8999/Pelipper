#include "smtpservice.h"

#include <QDebug>
#include <QFileInfo>

SmtpService::SmtpService(const QString &user,
                         const QString &password,
                         const QString &server,
                         const int port,
                         const QString &email,
                         const int timeout,
                         QObject *parent)
    : m_User(user)
    , m_Password(password)
    , m_Server(server)
    , m_Port(port)
    , m_Email(email)
    , m_Timeout(timeout)
    , QObject(parent)
{
}

SmtpService::~SmtpService()
{
}

bool SmtpService::Send(const QString &subject,
                       const QString &message,
                       const QString &htmlMessage,
                       const QList<Contact> &to,
                       const QList<Contact> &cc,
                       const QList<Contact> &bcc,
                       const QString &refMsgId,
                       const QList<QString> &attachmentPaths,
                       const bool flowed,
                       QString &resultMessage)
{
    const QString header = getHeader(subject, to, cc, bcc, refMsgId);
    const QString body = getBody(message, htmlMessage, attachmentPaths, flowed);
    const QString data = header + body;
    resultMessage = data;
    QList<Contact> recipients;
    recipients.append(to);
    recipients.append(cc);
    recipients.append(bcc);

    return SendMessage(data, recipients);
}

bool SmtpService::Send(const QString &data, const QList<Contact> &to, const QList<Contact> &cc, const QList<Contact> &bcc)
{
    QList<Contact> recipients;
    recipients.append(to);
    recipients.append(cc);
    recipients.append(bcc);
    return SendMessage(data, recipients);
}

QString SmtpService::getHeader(const QString &subject, const QList<Contact> &to, const QList<Contact> &cc, const QList<Contact> &bcc, const QString &refMsgId)
{
    QString name = MimeEncodeStr(m_User);
    struct mailimf_mailbox *mbfrom = mailimf_mailbox_new(strdup(name.toStdString().c_str()), strdup(m_Email.toStdString().c_str()));
    struct mailimf_mailbox_list *mblistfrom = mailimf_mailbox_list_new_empty();
    mailimf_mailbox_list_add(mblistfrom, mbfrom);

    clist *tolist = clist_new();
    for (auto &to_ : to) {
        QString toname = to_.Hostname();
        QString tonamemime = MimeEncodeStr(toname);
        QString toaddr = to_.Address();
        struct mailimf_mailbox *mbto =
            mailimf_mailbox_new(toname.isEmpty() ? NULL : strdup(tonamemime.toStdString().c_str()), strdup(toaddr.toStdString().c_str()));
        struct mailimf_address *addrto = mailimf_address_new(MAILIMF_ADDRESS_MAILBOX, mbto, NULL);
        clist_append(tolist, addrto);
    }

    struct mailimf_address_list *addrlistto = mailimf_address_list_new(tolist);

    struct mailimf_address_list *addrlistcc = NULL;
    if (!cc.empty()) {
        clist *cclist = clist_new();
        for (auto &cc_ : cc) {
            QString ccname = cc_.Hostname();
            QString ccnamemime = MimeEncodeStr(ccname);
            QString ccaddr = cc_.Address();
            struct mailimf_mailbox *mbcc =
                mailimf_mailbox_new(ccname.isEmpty() ? NULL : strdup(ccnamemime.toStdString().c_str()), strdup(ccaddr.toStdString().c_str()));
            struct mailimf_address *addrcc = mailimf_address_new(MAILIMF_ADDRESS_MAILBOX, mbcc, NULL);
            clist_append(cclist, addrcc);
        }

        addrlistcc = mailimf_address_list_new(cclist);
    }

    struct mailimf_address_list *addrlistbcc = NULL;
    // @todo: implement ui support for bcc
    if (!bcc.empty()) {
        clist *bcclist = clist_new();
        for (auto &bcc_ : bcc) {
            QString bccname = MimeEncodeStr(bcc_.Hostname());
            QString bccaddr = bcc_.Address();
            struct mailimf_mailbox *mbbcc = mailimf_mailbox_new(strdup(bccname.toStdString().c_str()), strdup(bccaddr.toStdString().c_str()));
            struct mailimf_address *addrbcc = mailimf_address_new(MAILIMF_ADDRESS_MAILBOX, mbbcc, NULL);
            clist_append(bcclist, addrbcc);
        }

        addrlistbcc = mailimf_address_list_new(bcclist);
    }

    struct mailimf_address_list *addrlistreplyto = NULL;

    QString subjectstr = MimeEncodeStr(subject);
    char *subjectcstr = strdup(subjectstr.toStdString().c_str());

    time_t now = time(NULL);
    struct mailimf_date_time *datenow = mailimf_get_date(now);

    char id[512];
    QString hostname = m_User;
    //    std::string appVersion = Util::GetMessageIdAppVersion();
    //    snprintf(id, sizeof(id), "%s.%lx.%lx.%x@%s", appVersion.c_str(), (long)now, random(), getpid(), hostname.c_str());
    char *messageid = strdup(id);

    clist *clist_inreplyto = NULL;
    clist *clist_references = NULL;
    if (!refMsgId.isEmpty()) {
        clist_inreplyto = clist_new();
        clist_append(clist_inreplyto, strdup(refMsgId.toStdString().c_str()));

        clist_references = clist_new();
        clist_append(clist_references, strdup(refMsgId.toStdString().c_str()));
    }

    struct mailimf_fields *fields = mailimf_fields_new_with_data_all(datenow,
                                                                     mblistfrom,
                                                                     NULL,
                                                                     addrlistreplyto,
                                                                     addrlistto,
                                                                     addrlistcc,
                                                                     addrlistbcc,
                                                                     messageid,
                                                                     clist_inreplyto,
                                                                     clist_references,
                                                                     subjectcstr);

    int col = 0;
    MMAPString *mmstr = mmap_string_new(NULL);
    mailimf_fields_write_mem(mmstr, &col, fields);
    std::string out = std::string(mmstr->str, mmstr->len);

    mmap_string_free(mmstr);
    mailimf_fields_free(fields);

    return QString::fromStdString(out);
}

QString SmtpService::getBody(const QString &message, const QString &htmlMessage, const QList<QString> &attachmentPaths, bool flowed)
{
    struct mailmime *mainMultipart = GetMimePart(mailmime_content_new_with_str("multipart/mixed"), mailmime_fields_new_empty(), 0);

    struct mailmime *htmlAndTextMultipart = GetMimePart(mailmime_content_new_with_str("multipart/alternative"), mailmime_fields_new_empty(), 0);

    struct mailmime *textBodyPart = GetMimeTextPart("text/plain", message, flowed);
    mailmime_smart_add_part(htmlAndTextMultipart, textBodyPart);

    if (!htmlMessage.isEmpty()) {
        struct mailmime *htmlBodyPart = GetMimeTextPart("text/html", htmlMessage, false);
        mailmime_smart_add_part(htmlAndTextMultipart, htmlBodyPart);
    }

    mailmime_smart_add_part(mainMultipart, htmlAndTextMultipart);

    for (auto &path : attachmentPaths) {
        if (QFileInfo(path).exists()) {
            struct mailmime *fileBodyPart = GetMimeFilePart(path);
            mailmime_smart_add_part(mainMultipart, fileBodyPart);
        } else {
            qDebug() << "file not exists";
        }
    }

    struct mailmime *msg_mime = mailmime_new_message_data(NULL);
    mailmime_smart_add_part(msg_mime, mainMultipart);

    int col = 0;
    MMAPString *mmstr = mmap_string_new(NULL);
    mailmime_write_mem(mmstr, &col, mainMultipart);
    std::string out = std::string(mmstr->str, mmstr->len);

    mmap_string_free(mmstr);
    mailmime_free(msg_mime);

    return QString::fromStdString(out);
}

bool SmtpService::SendMessage(const QString &data, const QList<Contact> &recipients)
{
    const bool enableSsl = (m_Port == 465);
    const bool enableTls = !enableSsl;
    const bool enableEsmtp = true;
    const bool enableLmtp = !enableEsmtp;

    mailsmtp *smtp = mailsmtp_new(0, NULL);
    if (smtp == NULL)
        return false;

    mailsmtp_set_timeout(smtp, m_Timeout);

    int rv = MAILSMTP_NO_ERROR;
    if (enableSsl) {
        rv = mailsmtp_ssl_connect(smtp, m_Server.toStdString().c_str(), m_Port);
    } else {
        rv = mailsmtp_socket_connect(smtp, m_Server.toStdString().c_str(), m_Port);
    }

    if (rv != MAILSMTP_NO_ERROR)
        return false;

    bool esmtpMode = false;
    const QString hostname = m_User;
    if (enableLmtp) {
        rv = mailesmtp_lhlo(smtp, hostname.toStdString().c_str());
    } else if (enableEsmtp && (rv = mailesmtp_ehlo(smtp)) == MAILSMTP_NO_ERROR) {
        esmtpMode = true;
    } else if (!enableEsmtp || (rv == MAILSMTP_ERROR_NOT_IMPLEMENTED)) {
        rv = mailsmtp_helo(smtp);
    }

    if (rv != MAILSMTP_NO_ERROR)
        return false;

    if (esmtpMode && enableTls) {
        rv = mailsmtp_socket_starttls(smtp);

        if (rv != MAILSMTP_NO_ERROR)
            return false;

        if (enableLmtp) {
            rv = mailesmtp_lhlo(smtp, hostname.toStdString().c_str());
        } else if (enableEsmtp && (rv = mailesmtp_ehlo(smtp)) == MAILSMTP_NO_ERROR) {
            esmtpMode = true;
        } else if (!enableEsmtp || rv == MAILSMTP_ERROR_NOT_IMPLEMENTED) {
            rv = mailsmtp_helo(smtp);
        }

        if (rv != MAILSMTP_NO_ERROR)
            return false;
    }

    if (esmtpMode) {
        rv = mailsmtp_auth(smtp, m_Email.toStdString().c_str(), m_Password.toStdString().c_str());

        if (rv != MAILSMTP_NO_ERROR)
            return false;
    }

    static int msgid = 0;
    std::string envid = std::to_string(++msgid) + std::string("@") + hostname.toStdString();

    if (esmtpMode) {
        rv = mailesmtp_mail(smtp, m_Email.toStdString().c_str(), 1, envid.c_str());
    } else {
        rv = mailsmtp_mail(smtp, m_Email.toStdString().c_str());
    }

    if (rv != MAILSMTP_NO_ERROR)
        return false;

    clist *recipientsClist = clist_new();
    for (auto &recipient : recipients) {
        char *r = strdup(recipient.Address().toStdString().c_str());
        if (esmtpMode) {
            rv = mailesmtp_rcpt(smtp, r, MAILSMTP_DSN_NOTIFY_FAILURE | MAILSMTP_DSN_NOTIFY_DELAY, NULL);
        } else {
            rv = mailsmtp_rcpt(smtp, r);
        }

        if (rv != MAILSMTP_NO_ERROR)
            return false;

        clist_append(recipientsClist, (void *)r);

        free(r);
    }

    rv = mailsmtp_data(smtp);

    if (rv != MAILSMTP_NO_ERROR)
        return false;

    if (enableLmtp) {
        int *retcodes = (int *)malloc((clist_count(recipientsClist) * sizeof(int)));

        rv = maillmtp_data_message(smtp, data.toStdString().c_str(), data.size(), recipientsClist, retcodes);

        if (rv != MAILSMTP_NO_ERROR)
            return false;

        //        for (int i = 0; i < clist_count(recipients); i++) {
        //            LOG_WARNING("recipient \"%s\" returned %d", (char *)clist_nth_data(recipients, i), retcodes[i]);
        //        }

        free(retcodes);
    } else {
        rv = mailsmtp_data_message(smtp, data.toStdString().c_str(), data.size());

        if (rv != MAILSMTP_NO_ERROR)
            return false;
    }

    clist_free(recipientsClist);
    mailsmtp_free(smtp);

    return true;
}

mailmime *SmtpService::GetMimeTextPart(const char *mimeType, const QString &message, bool flowed)
{
    int encodingType = MAILMIME_MECHANISM_QUOTED_PRINTABLE;
    struct mailmime_mechanism *encoding = mailmime_mechanism_new(encodingType, NULL);
    struct mailmime_disposition *disposition = mailmime_disposition_new_with_data(MAILMIME_DISPOSITION_TYPE_INLINE, NULL, NULL, NULL, NULL, (size_t)-1);
    struct mailmime_fields *mimefields = mailmime_fields_new_with_data(encoding, NULL, NULL, disposition, NULL);
    struct mailmime_content *content = mailmime_content_new_with_str(mimeType);
    char *paramkey = strdup("charset");
    char *paramval = strdup("utf-8");
    struct mailmime_parameter *param = mailmime_param_new_with_data(paramkey, paramval);
    clist_append(content->ct_parameters, param);
    if (flowed) {
        char *formatkey = strdup("format");
        char *formatval = strdup("flowed");
        struct mailmime_parameter *formatparam = mailmime_param_new_with_data(formatkey, formatval);
        clist_append(content->ct_parameters, formatparam);
    }
    struct mailmime *mime = GetMimePart(content, mimefields, 1);
    mailmime_set_body_text(mime, const_cast<char *>(message.toStdString().c_str()), message.size());

    free(paramkey);
    free(paramval);

    return mime;
}

mailmime *SmtpService::GetMimeFilePart(const QString &path, const QString &mimeType)
{
    const QString filename = QFileInfo(path).baseName();
    char *dispositionname = strdup(filename.toStdString().c_str());
    int encodingtype = MAILMIME_MECHANISM_BASE64;
    struct mailmime_disposition *disposition =
        mailmime_disposition_new_with_data(MAILMIME_DISPOSITION_TYPE_ATTACHMENT, dispositionname, NULL, NULL, NULL, (size_t)-1);
    struct mailmime_mechanism *encoding = mailmime_mechanism_new(encodingtype, NULL);
    struct mailmime_content *content = mailmime_content_new_with_str(mimeType.toStdString().c_str());
    struct mailmime_fields *mimefields = mailmime_fields_new_with_data(encoding, NULL, NULL, disposition, NULL);
    struct mailmime *mime = GetMimePart(content, mimefields, 1);
    mailmime_set_body_file(mime, strdup(path.toStdString().c_str()));

    return mime;
}

mailmime *SmtpService::GetMimePart(mailmime_content *p_Content, mailmime_fields *p_MimeFields, bool p_ForceSingle)
{
    clist *list = NULL;
    int mime_type = -1;

    if (p_ForceSingle) {
        mime_type = MAILMIME_SINGLE;
    } else {
        switch (p_Content->ct_type->tp_type) {
        case MAILMIME_TYPE_DISCRETE_TYPE:
            mime_type = MAILMIME_SINGLE;
            break;

        case MAILMIME_TYPE_COMPOSITE_TYPE:
            switch (p_Content->ct_type->tp_data.tp_composite_type->ct_type) {
            case MAILMIME_COMPOSITE_TYPE_MULTIPART:
                mime_type = MAILMIME_MULTIPLE;
                break;

            case MAILMIME_COMPOSITE_TYPE_MESSAGE:
                if (strcasecmp(p_Content->ct_subtype, "rfc822") == 0) {
                    mime_type = MAILMIME_MESSAGE;
                } else {
                    mime_type = MAILMIME_SINGLE;
                }
                break;

            default:
                return NULL;
            }
            break;

        default:
            return NULL;
        }
    }

    if (mime_type == MAILMIME_MULTIPLE) {
        list = clist_new();
        if (list == NULL) {
            return NULL;
        }

        char *attr_name = strdup("boundary");
        char id[512];
        //        snprintf(id, sizeof(id), "%llx_%lx_%x", (long long)time(NULL), random(), getpid());

        char *attr_value = strdup(id);
        if (attr_name == NULL) {
            clist_free(list);
            free(attr_name);
            return NULL;
        }

        struct mailmime_parameter *param = mailmime_parameter_new(attr_name, attr_value);
        if (param == NULL) {
            clist_free(list);
            free(attr_value);
            free(attr_name);
            return NULL;
        }

        clist *parameters = NULL;
        if (p_Content->ct_parameters == NULL) {
            parameters = clist_new();
            if (parameters == NULL) {
                clist_free(list);
                mailmime_parameter_free(param);
                return NULL;
            }
        } else {
            parameters = p_Content->ct_parameters;
        }

        if (clist_append(parameters, param) != 0) {
            clist_free(list);
            clist_free(parameters);
            mailmime_parameter_free(param);
            return NULL;
        }

        if (p_Content->ct_parameters == NULL) {
            p_Content->ct_parameters = parameters;
        }
    }

    struct mailmime *mime = mailmime_new(mime_type, NULL, 0, p_MimeFields, p_Content, NULL, NULL, NULL, list, NULL, NULL);
    if (mime == NULL) {
        clist_free(list);
    }

    return mime;
}

QString SmtpService::MimeEncodeStr(const QString &p_In)
{
    if (std::all_of(p_In.cbegin(), p_In.cend(), [](char c) {
            return isprint(c);
        })) {
        return p_In;
    }

    int col = 0;
    MMAPString *mmstr = mmap_string_new(NULL);
    mailmime_quoted_printable_write_mem(mmstr, &col, 1, p_In.toStdString().c_str(), p_In.size());
    std::string out = std::string("=?UTF-8?Q?") + std::string(mmstr->str, mmstr->len) + std::string("?=");
    mmap_string_free(mmstr);
    return QString::fromStdString(out);
}
