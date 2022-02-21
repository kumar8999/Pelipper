#include "message.h"

#include <QDebug>

Message::Message(const QString &accountEmail, ssize_t uid, QObject *parent)
    : m_AccountEmail(accountEmail)
    , m_Uid(uid)
    , QObject(parent)
{
    m_from = nullptr;
    m_flags = nullptr;
}

void Message::setHeaderData(const QString &data)
{
    m_headerData = data;
    parseHeader();
}

void Message::parseHeader()
{
    mailmime *mime = NULL;
    size_t index = 0;
    int r = mailmime_parse(m_headerData.toStdString().c_str(), m_headerData.size(), &index, &mime);

    if (r != MAILIMF_NO_ERROR) {
        qDebug() << "Could not parse" << r;
        mailmime_free(mime);
        return;
    }

    if (mime->mm_type == MAILMIME_MESSAGE) {
        if (mime->mm_data.mm_message.mm_fields) {
            mailimf_fields *fields = mime->mm_data.mm_message.mm_fields;

            if (clist_begin(fields->fld_list) != NULL) {
                for (clistiter *it = clist_begin(fields->fld_list); it != NULL;
                     it = clist_next(it)) {
                    mailimf_field *field = (mailimf_field *) clist_content(it);

                    switch (field->fld_type) {
                    case MAILIMF_FIELD_ORIG_DATE: {
                        struct mailimf_date_time *datetime = field->fld_data.fld_orig_date
                                                                 ->dt_date_time;
                        QDate date = QDate(datetime->dt_year, datetime->dt_month, datetime->dt_day);
                        QTime time = QTime(datetime->dt_hour, datetime->dt_min, datetime->dt_sec);
                        m_DateTime = QDateTime(date, time);
                        break;
                    }
                    case MAILIMF_FIELD_FROM: {
                        m_from = parseFrom(field->fld_data.fld_from);
                        break;
                    }

                    case MAILIMF_FIELD_TO: {
                        m_To = parseTo(field->fld_data.fld_to);
                        break;
                    }

                    case MAILIMF_FIELD_CC: {
                        m_cc = parseAddressList(field->fld_data.fld_cc->cc_addr_list);
                        break;
                    }

                    case MAILIMF_FIELD_BCC: {
                        m_Bcc = parseAddressList(field->fld_data.fld_bcc->bcc_addr_list);
                        break;
                    }

                    case MAILIMF_FIELD_SUBJECT: {
                        m_Subject = mimeToUtf8(field->fld_data.fld_subject->sbj_value);
                        break;
                    }

                    case MAILIMF_FIELD_MESSAGE_ID:
                        m_MsgID = mimeToUtf8(field->fld_data.fld_message_id->mid_value);
                        break;

                    case MAILIMF_FIELD_REPLY_TO: {
                        m_ReplyTo = parseAddressList(field->fld_data.fld_reply_to->rt_addr_list);
                        break;
                    }
                    default:
                        break;
                    }
                }
            }
        }
    }

    mailmime_free(mime);
}

void Message::setBodyData(const QString &data)
{
    m_data = data;
    m_headerData = data;
    parseHeader();
    parseBody();
}

void Message::parseBody()
{
    struct mailmime *mime = NULL;
    size_t index = 0;

    int r = mailmime_parse(m_data.toStdString().c_str(), m_data.size(), &index, &mime);

    if (r != MAILIMF_NO_ERROR) {
        qDebug() << "parse body error code" << r;
        mailmime_free(mime);
        return;
    }

    parseMime(mime, 0);
    parseHtml();
    parsePlainText();

    mailmime_free(mime);
}

ssize_t Message::Uid() const
{
    return m_Uid;
}

void Message::setUid(ssize_t newUid)
{
    m_Uid = newUid;
}

Contact *Message::parseFrom(mailimf_from *mbList)
{
    Contact *mailbox;
    mailbox = parseMailBoxList(mbList->frm_mb_list).at(0);

    return mailbox;
}

QList<Contact *> Message::parseTo(mailimf_to *mbList)
{
    return parseAddressList(mbList->to_addr_list);
}

QList<Contact *> Message::parseMailBoxList(mailimf_mailbox_list *mbList)
{
    clistiter *cur;
    QList<Contact *> contacts;

    for (cur = clist_begin(mbList->mb_list); cur != NULL; cur = clist_next(cur)) {
        struct mailimf_mailbox *mb;
        mb = (mailimf_mailbox *) clist_content(cur);
        Contact *contact = new Contact();

        if (mb->mb_display_name != NULL)
            contact->setHostname(mimeToUtf8(mb->mb_display_name));

        contact->setAddress(mimeToUtf8(mb->mb_addr_spec));

        contacts.append(contact);
    }

    return contacts;
}

QList<Contact *> Message::parseAddressList(mailimf_address_list *addressList)
{
    clistiter *cur;
    QList<Contact *> addrList;

    for (cur = clist_begin(addressList->ad_list); cur != NULL; cur = clist_next(cur)) {
        struct mailimf_address *addr = (mailimf_address *) clist_content(cur);

        addrList = parseAddress(addr);
    }

    return addrList;
}

QList<Contact *> Message::parseAddress(mailimf_address *addr)
{
    QList<Contact *> contacts;

    switch (addr->ad_type) {
    case MAILIMF_ADDRESS_GROUP: {
        clistiter *it;

        for (it = clist_begin(addr->ad_data.ad_group->grp_mb_list->mb_list); it != NULL;
             it = clist_next(it)) {
            struct mailimf_mailbox *mb;

            mb = (mailimf_mailbox *) clist_content(it);
            Contact *contact = new Contact();
            if (mb->mb_display_name != NULL) {
                contact->setHostname(mimeToUtf8(mb->mb_display_name));
            }
            contact->setAddress(mimeToUtf8(mb->mb_addr_spec));
            contacts.append(contact);
        }
    } break;

    case MAILIMF_ADDRESS_MAILBOX: {
        struct mailimf_mailbox *mb;
        mb = addr->ad_data.ad_mailbox;
        Contact *contact = new Contact();
        if (mb->mb_display_name != NULL) {
            contact->setHostname(mimeToUtf8(mb->mb_display_name));
        }
        contact->setAddress(mimeToUtf8(mb->mb_addr_spec));
        contacts.append(contact);
    } break;
    }

    return contacts;
}

QString Message::getMimeType(mailmime_content *content)
{
    mailmime_type *type = content->ct_type;
    QString mimeType;

    switch (type->tp_type) {
    case MAILMIME_TYPE_DISCRETE_TYPE:
        mimeType = getDiscreteMimeType(type->tp_data.tp_discrete_type);
        break;
    case MAILMIME_TYPE_COMPOSITE_TYPE:
        mimeType = getCompositeMimeType(type->tp_data.tp_composite_type);
        break;
    default:
        mimeType = "";
        break;
    }

    if (mimeType.isEmpty()) {
        mimeType = "application";
    }

    mimeType = mimeType + "/" + QString::fromStdString(content->ct_subtype);
    mimeType = mimeType.toLower();

    return mimeType;
}

QString Message::getDiscreteMimeType(mailmime_discrete_type *type)
{
    switch (type->dt_type) {
    case MAILMIME_DISCRETE_TYPE_TEXT:
        return "text";
    case MAILMIME_DISCRETE_TYPE_IMAGE:
        return "image";
    case MAILMIME_DISCRETE_TYPE_AUDIO:
        return "audio";
    case MAILMIME_DISCRETE_TYPE_VIDEO:
        return "video";
    case MAILMIME_DISCRETE_TYPE_APPLICATION:
        return "application";
    case MAILMIME_DISCRETE_TYPE_EXTENSION:
        return QString::fromStdString(type->dt_extension);
        break;
    default:
        return "";
    }
}

QString Message::getCompositeMimeType(mailmime_composite_type *type)
{
    switch (type->ct_type) {
    case MAILMIME_COMPOSITE_TYPE_MESSAGE:
        return "message";
    case MAILMIME_COMPOSITE_TYPE_MULTIPART:
        return "multipart";
    case MAILMIME_COMPOSITE_TYPE_EXTENSION:
        return QString::fromStdString(type->ct_token);
        break;
    default:
        return "";
    }
}

void Message::parseMime(mailmime *mime, int depth)
{
    mailmime_content *contentType = mime->mm_content_type;
    mailmime_type *type = contentType->ct_type;
    QString mimeType = getMimeType(mime->mm_content_type);

    switch (mime->mm_type) {
    case MAILMIME_SINGLE:
        parseSingleMime(mime, mimeType);
        break;
    case MAILMIME_MULTIPLE:
        for (clistiter *cur = clist_begin(mime->mm_data.mm_multipart.mm_mp_list); cur != NULL;
             cur = clist_next(cur)) {
            parseMime((mailmime *) clist_content(cur), depth + 1);
        }
        break;
    case MAILMIME_MESSAGE:
        if (mime->mm_data.mm_message.mm_fields) {
            if (clist_begin(mime->mm_data.mm_message.mm_fields->fld_list) != NULL) {
                //                printf("headers begin\n");

                //                printf("headers end\n");
            }

            if (mime->mm_data.mm_message.mm_msg_mime != NULL) {
                parseMime(mime->mm_data.mm_message.mm_msg_mime, depth + 1);
            }
            break;
        }
    }
}

void Message::parseSingleMime(mailmime *mime, const QString &mimeType)
{
    QString fileName;
    QString charset;
    bool isAttachment = false;

    parseMimeFields(mime, fileName, charset, isAttachment);

    mailmime_data *data = mime->mm_data.mm_single;
    if (data == NULL) {
        return;
    }

    switch (data->dt_type) {
    case MAILMIME_DATA_TEXT: {
        size_t index = 0;
        char *parsedStr = NULL;
        size_t parsedLen = 0;
        int rv = mailmime_part_parse(data->dt_data.dt_text.dt_data,
                                     data->dt_data.dt_text.dt_length,
                                     &index,
                                     data->dt_encoding,
                                     &parsedStr,
                                     &parsedLen);

        if (rv == MAILIMF_NO_ERROR) {
            MessagePart *bodypart = new MessagePart();
            bodypart->m_Charset = charset;
            bodypart->m_MimeType = mimeType;
            bodypart->m_Size = std::string(parsedStr, parsedLen).size();

            if (isAttachment) {
                bodypart->m_IsAttachment = isAttachment;
                bodypart->m_Payload = QByteArray(parsedStr, parsedLen);
                bodypart->m_Filename = fileName;
            } else {
                if (mimeType == "text/html") {
                    bodypart->m_TextHtml = QString::fromStdString(parsedStr);

                } else if (mimeType == "text/plain") {
                    bodypart->m_PlainText = QString::fromStdString(parsedStr);
                }
            }
            m_BodyParts.append(bodypart);
        }

    } break;

    case MAILMIME_DATA_FILE:
        break;

    default:
        break;
    }
}

void Message::parseMimeFields(mailmime *mime,
                              QString &filename,
                              QString &charset,
                              bool &isattachment)
{
    mailmime_single_fields fields;
    memset(&fields, 0, sizeof(mailmime_single_fields));
    if (mime->mm_mime_fields != NULL) {
        mailmime_single_fields_init(&fields, mime->mm_mime_fields, mime->mm_content_type);

        if (fields.fld_disposition != NULL) {
            struct mailmime_disposition_type *type = fields.fld_disposition->dsp_type;
            if (type != NULL) {
                isattachment = (type->dsp_type == MAILMIME_DISPOSITION_TYPE_ATTACHMENT);
            }
        }

        if (fields.fld_disposition_filename != NULL) {
            filename = QString::fromStdString(fields.fld_disposition_filename);
        } else if (fields.fld_content_name != NULL) {
            filename = QString::fromStdString(fields.fld_content_name);
        }

        if (fields.fld_content_charset != NULL) {
            charset = QString::fromStdString(fields.fld_content_charset).toLower();
        }
    }
}

void Message::parseHtml()
{
    QString html = "";
    for (auto bodypart : qAsConst(m_BodyParts)) {
        if (bodypart->m_TextHtml != "") {
            html.append(bodypart->m_TextHtml);
        }
    }

    m_Html = html;
}

void Message::parsePlainText()
{
    QString text = "";
    for (auto bodypart : qAsConst(m_BodyParts)) {
        if (bodypart->m_PlainText != "") {
            text.append(bodypart->m_PlainText);
        }
    }

    m_PlainText = text;
}

QString Message::mimeToUtf8(std::string str)
{
    const char *charset = "UTF-8";
    char *cdecoded = NULL;
    size_t curtoken = 0;
    int rv = mailmime_encoded_phrase_parse(charset,
                                           str.c_str(),
                                           str.size(),
                                           &curtoken,
                                           charset,
                                           &cdecoded);
    if ((rv == MAILIMF_NO_ERROR) && (cdecoded != NULL)) {
        std::string decoded(cdecoded);
        free(cdecoded);
        return QString::fromStdString(decoded);
    } else {
        return QString::fromStdString(str);
    }
}

Flags *Message::flags() const
{
    return m_flags;
}

void Message::setFlags(Flags *newFlags)
{
    m_flags = newFlags;
}

const QString &Message::Data() const
{
    return m_data;
}

const QString &Message::PlainText() const
{
    return m_PlainText;
}

const QString &Message::Html() const
{
    return m_Html;
}

const QList<Contact *> &Message::ReplyTo() const
{
    return m_ReplyTo;
}

bool Message::hasAttachment()
{
    for (auto bodypart : m_BodyParts) {
        if (bodypart->m_IsAttachment) {
            return true;
        }
    }
    return false;
}

int Message::attachmentCount()
{
    int count = 0;
    for (auto bodypart : m_BodyParts) {
        if (bodypart->m_IsAttachment) {
            count++;
        }
    }
    return count;
}

const QList<Contact *> &Message::Bcc() const
{
    return m_Bcc;
}

const QList<Contact *> &Message::cc() const
{
    return m_cc;
}

const QList<Contact *> &Message::To() const
{
    return m_To;
}

const QString &Message::MsgID() const
{
    return m_MsgID;
}

Contact *Message::From() const
{
    return m_from;
}

const QString &Message::Subject() const
{
    return m_Subject;
}

const QDateTime &Message::DateTime() const
{
    return m_DateTime;
}

const QString &Message::AccountEmail() const
{
    return m_AccountEmail;
}

void Message::setAccountEmail(const QString &newAccountEmail)
{
    m_AccountEmail = newAccountEmail;
}
