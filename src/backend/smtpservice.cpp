#include "smtpservice.h"

#include <libetpan/libetpan.h>

#include <QDebug>
#include <QFileInfo>
#include <QMimeDatabase>

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

bool SmtpService::Send(const QString &subject,
                       const QString &htmlMessage,
                       const QList<Address> &to,
                       const QList<Address> &cc,
                       const QList<Address> &bcc,
                       const QStringList &attachmentPaths)
{
    // from field;
    //    mailimf_mailbox_list *from;
    //    from = mailimf_mailbox_list_new_empty();

    //    int r = mailimf_mailbox_list_add_mb(from, strdup(m_User.toStdString().c_str()), strdup(m_Email.toStdString().c_str()));

    //    //    to field
    //    mailimf_address_list *toField = createAddressField(to);

    //    //    cc field
    //    mailimf_address_list *ccField = cc.isEmpty() ? NULL : createAddressField(cc);

    //    //    bcc field
    //    mailimf_address_list *bccField = bcc.isEmpty() ? NULL : createAddressField(bcc);

    //    char *subjectcstr = strdup(subject.toStdString().c_str());

    //    mailimf_fields *fields = mailimf_fields_new_with_data(from /* from */,
    //                                                          NULL /* sender */,
    //                                                          NULL /* reply-to */,
    //                                                          toField,
    //                                                          ccField /* cc */,
    //                                                          bccField /* bcc */,
    //                                                          NULL /* in-reply-to */,
    //                                                          NULL /* references */,
    //                                                          subjectcstr);

    //    // mainmime part
    //    mailmime *mime = mailmime_new_empty(mailmime_content_new_with_str("multipart/alternative"), mailmime_fields_new_empty());

    //    // html mime part
    //    mailmime *part = mailmime_new_empty(mailmime_content_new_with_str("text/html"), mailmime_fields_new_encoding(MAILMIME_MECHANISM_QUOTED_PRINTABLE));
    //    mailmime_set_body_text(part, const_cast<char *>(htmlMessage.toStdString().c_str()), htmlMessage.toStdString().size());
    //    mailmime_smart_add_part(mime, part);

    //    // attachment mime part
    //    for (const auto &attachmentPath : attachmentPaths) {
    //        QFileInfo atthFileInfo = QFileInfo(attachmentPath);

    //        if (!atthFileInfo.exists()) {
    //            continue;
    //        }

    //        char *dispositionname = strdup(atthFileInfo.baseName().toStdString().c_str());
    //        int encodingtype = MAILMIME_MECHANISM_BASE64;
    //        mailmime_disposition *disposition =
    //            mailmime_disposition_new_with_data(MAILMIME_DISPOSITION_TYPE_ATTACHMENT, dispositionname, NULL, NULL, NULL, (size_t)-1);
    //        mailmime_mechanism *encoding = mailmime_mechanism_new(encodingtype, NULL);

    //        QString mimeType = getMimeType(attachmentPath);
    //        if (mimeType.isEmpty()) {
    //            mimeType = "application/octet-stream";
    //        }

    //        mailmime_content *content = mailmime_content_new_with_str(mimeType.toStdString().c_str());
    //        mailmime_fields *mimefields = mailmime_fields_new_with_data(encoding, NULL, NULL, disposition, NULL);
    //        mailmime *attachPart = mailmime_new_empty(content, mimefields);
    //        mailmime_set_body_file(attachPart, strdup(attachmentPath.toStdString().c_str()));
    //        mailmime_smart_add_part(mime, attachPart);
    //    }

    //    mailmime *msg_mime = mailmime_new_message_data(NULL);
    //    mailmime_smart_add_part(msg_mime, mime);

    //    int col = 0;
    //    MMAPString *mmstr = mmap_string_new(NULL);
    //    mailmime_write_mem(mmstr, &col, mime);
    //    std::string out = std::string(mmstr->str, mmstr->len);

    //    mmap_string_free(mmstr);
    //    mailmime_free(msg_mime);

    //    qDebug() << QString::fromStdString(out);

    //    SendMessage(QString::fromStdString(out), to);

    return false;
}

bool SmtpService::SendMessage(const QString &data, const QList<Address> &recipientList)
{
    int s = -1;
    int ret, i;
    char **r;
    mailsmtp *smtp = NULL;
    int *retcodes = NULL;
    bool enableEsmtp = true;
    bool enableLmtp = !enableEsmtp;
    bool enableTls = false;

    smtp = mailsmtp_new(0, NULL);

    /* first open the stream */
    if (true) {
        // use SMTP over SSL
        ret = mailsmtp_ssl_connect(smtp, m_Server.toStdString().c_str(), m_Port);
    } else {
        // use STARTTLS
        ret = mailsmtp_socket_connect(smtp, m_Server.toStdString().c_str(), m_Port);
    }

    if (ret != MAILSMTP_NO_ERROR) {
        qDebug() << "mail error 138" << m_Server << m_Port;
        return false;
    }

    bool esmtpMode = false;

    /* then introduce ourselves */
    if (enableLmtp) {
        ret = mailesmtp_lhlo(smtp, "lmtp-test");
    } else if (enableEsmtp && (ret = mailesmtp_ehlo(smtp)) == MAILSMTP_NO_ERROR) {
        esmtpMode = true;
    } else if (!enableEsmtp || (ret == MAILSMTP_ERROR_NOT_IMPLEMENTED)) {
        ret = mailsmtp_helo(smtp);
    }

    if (ret != MAILSMTP_NO_ERROR) {
        qDebug() << "mail error 155";
        return false;
    }

    //    if (esmtpMode && enableTls && (ret = mailsmtp_socket_starttls(smtp)) != MAILSMTP_NO_ERROR) {
    //        qDebug() << "mail error 158";
    //        return false;
    //    }

    //    if (esmtpMode && enableTls) {
    //        /* introduce ourselves again */
    //        if (enableLmtp)
    //            ret = mailesmtp_lhlo(smtp, "lmtp-test");
    //        else if (enableEsmtp && (ret = mailesmtp_ehlo(smtp)) == MAILSMTP_NO_ERROR)
    //            esmtpMode = true;
    //        else if (!enableEsmtp || ret == MAILSMTP_ERROR_NOT_IMPLEMENTED)
    //            ret = mailsmtp_helo(smtp);
    //        if (ret != MAILSMTP_NO_ERROR) {
    //            qDebug() << "mail error 171";
    //            return false;
    //        }
    //    }

    qDebug() << "here 175";

    if (esmtpMode && enableTls) {
        ret = mailsmtp_socket_starttls(smtp);

        if (ret != MAILSMTP_NO_ERROR)
            return false;

        if (enableLmtp) {
            ret = (mailesmtp_lhlo(smtp, "lmtp-test"));
        } else if (enableEsmtp && ((ret = (mailesmtp_ehlo(smtp))) == MAILSMTP_NO_ERROR)) {
            esmtpMode = true;
        } else if (!enableEsmtp || ret == MAILSMTP_ERROR_NOT_IMPLEMENTED) {
            ret = (mailsmtp_helo(smtp));
        }

        if (ret != MAILSMTP_NO_ERROR)
            return false;
    }

    qDebug() << "here 195";

    /* source */

    ret = (mailsmtp_auth(smtp, m_Email.toStdString().c_str(), m_Password.toStdString().c_str()));

    if (ret != MAILSMTP_NO_ERROR) {
        qDebug() << "mail error 198";
        return false;
    }

    if (esmtpMode) {
        ret = mailesmtp_mail(smtp, m_Email.toStdString().c_str(), 1, "etPanSMTPTest");
    } else {
        ret = mailsmtp_mail(smtp, m_Email.toStdString().c_str());
    }

    if (ret != MAILSMTP_NO_ERROR) {
        qDebug() << "mail error 186" << esmtpMode << ret;
        return false;
    }

    /* recipients */
    clist *recipients = clist_new();
    //    for (auto recipient : recipientList) {
    //        char *r = strdup(recipient.address().toStdString().c_str());
    //        if (esmtpMode) {
    //            ret = mailesmtp_rcpt(smtp, r, MAILSMTP_DSN_NOTIFY_FAILURE | MAILSMTP_DSN_NOTIFY_DELAY, NULL);
    //        } else {
    //            ret = mailsmtp_rcpt(smtp, r);
    //        }

    //        if (ret != MAILSMTP_NO_ERROR) {
    //            qDebug() << "mail error 200";
    //            return false;
    //        }

    //        clist_append(recipients, (void *)r);

    //        free(r);
    //    }

    //    qDebug() << "mail error 236";

    //    /* message */
    //    if ((ret = mailsmtp_data(smtp)) != MAILSMTP_NO_ERROR) {
    //        return false;
    //    }

    //    if (enableLmtp) {
    //        retcodes = (int *)malloc((clist_count(recipients) * sizeof(int)));
    //        ret = maillmtp_data_message(smtp, data.toStdString().c_str(), data.toStdString().size(), recipients, retcodes);

    //        if (ret != MAILSMTP_NO_ERROR) {
    //            qDebug() << "mail error 248";
    //            return false;
    //        }

    //    } else {
    //        ret = mailsmtp_data_message(smtp, data.toStdString().c_str(), data.toStdString().size());

    //        if (ret != MAILSMTP_NO_ERROR) {
    //            qDebug() << "mail error 255";
    //            return false;
    //        }
    //    }

    qDebug() << "sending";
    clist_free(recipients);
    mailsmtp_free(smtp);
    return 0;
}

mailimf_address_list *SmtpService::createAddressField(QList<Address> contactList)
{
    mailimf_address_list *addrList = mailimf_address_list_new_empty();
    //    for (const auto &contact : contactList) {
    //        std::string dispName = contact.displayName().toStdString();
    //        std::string addr = contact.address().toStdString();
    //        char *dispNamechr = dispName.empty() ? NULL : strdup(dispName.c_str());
    //        mailimf_address_list_add_mb(addrList, dispNamechr, strdup(addr.c_str()));
    //    }

    return addrList;
}

QString SmtpService::getMimeType(const QString &filePath)
{
    return QMimeDatabase().mimeTypeForFile(filePath).name();
}
