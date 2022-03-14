#include "message.h"

#include <QDebug>

Message::Message(const QString &accountEmail, ssize_t uid, QObject *parent)
    : m_accountEmail(accountEmail)
    , m_uid(uid)
    , QObject(parent)
{
}

void Message::parseFromData(const QString &data)
{
    m_data = data;
    parseData();
}

void Message::parseHeaderFields(mailimf_fields *headerFields)
{
    if (headerFields) {
        mailimf_single_fields singleFields;
        mailimf_single_fields_init(&singleFields, headerFields);

        m_header = new MessageHeader();

        // date field
        if (singleFields.fld_orig_date != NULL) {
            mailimf_orig_date *fldDate = singleFields.fld_orig_date;
            mailimf_date_time *datetime = fldDate->dt_date_time;

            QDate date = QDate(datetime->dt_year, datetime->dt_month, datetime->dt_day);
            QTime time = QTime(datetime->dt_hour, datetime->dt_min, datetime->dt_sec);

            qDebug() << "date time" << QDateTime(date, time);
            m_header->setDateTime(QDateTime(date, time));
        }

        // from field
        if (singleFields.fld_from != NULL) {
            mailimf_from *fromField = singleFields.fld_from;
            Address *from = parseFrom(fromField);
            m_header->setFrom(from);
        }

        // sender field
        if (singleFields.fld_sender != NULL) {
            mailimf_sender *senderField = singleFields.fld_sender;
            m_header->setSender(parseSender(senderField->snd_mb));
        }

        // reply to field
        if (singleFields.fld_reply_to != NULL) {
            mailimf_reply_to *replyField = singleFields.fld_reply_to;
            m_header->setReplyTo(parseAddressList(replyField->rt_addr_list));
        }

        // to field
        if (singleFields.fld_to != NULL) {
            mailimf_to *toField = singleFields.fld_to;
            m_header->setTo(parseAddressList(toField->to_addr_list));
        }

        if (singleFields.fld_cc != NULL) {
            mailimf_cc *ccField = singleFields.fld_cc;
            m_header->setCc(parseAddressList(ccField->cc_addr_list));
        }

        if (singleFields.fld_bcc != NULL) {
            mailimf_bcc *bccField = singleFields.fld_bcc;
            m_header->setBcc(parseAddressList(bccField->bcc_addr_list));
        }

        if (singleFields.fld_message_id != NULL) {
            mailimf_message_id *messageIdField = singleFields.fld_message_id;
            m_header->setMessageId(mimeToUtf8(messageIdField->mid_value));
        }

        if (singleFields.fld_in_reply_to != NULL) {
            mailimf_in_reply_to *inreplyToField = singleFields.fld_in_reply_to;
        }

        if (singleFields.fld_references != NULL) {
            mailimf_references *referencesField = singleFields.fld_references;
        }

        if (singleFields.fld_subject != NULL) {
            mailimf_subject *subjectField = singleFields.fld_subject;
            m_header->setSubject(mimeToUtf8(subjectField->sbj_value));
        }

        if (singleFields.fld_comments != NULL) {
            mailimf_comments *commentField = singleFields.fld_comments;
        }

        if (singleFields.fld_keywords != NULL) {
            mailimf_keywords *keywordsField = singleFields.fld_keywords;
        }
    }
}

void Message::parseData()
{
    struct mailmime *mime = NULL;
    size_t index = 0;

    int r = mailmime_parse(m_data.toStdString().c_str(), m_data.size(), &index, &mime);

    if (r != MAILIMF_NO_ERROR) {
        qDebug() << "parse message error code" << r;
        mailmime_free(mime);
        return;
    }

    parseMime(mime);

    mailmime_free(mime);
}

Address *Message::parseFrom(mailimf_from *mbList)
{
    Address *mailbox;
    mailbox = parseMailBoxList(mbList->frm_mb_list)->at(0);

    return mailbox;
}

Address *Message::parseSender(mailimf_mailbox *mb)
{
    Address *mailbox = new Address();

    if (mb->mb_display_name != NULL) {
        mailbox->setDisplayName(QString::fromStdString(mb->mb_display_name));
    }
    mailbox->setAddress(QString::fromStdString(mb->mb_addr_spec));

    return mailbox;
}

QList<Address *> *Message::parseTo(mailimf_to *mbList)
{
    return parseAddressList(mbList->to_addr_list);
}

QList<Address *> *Message::parseMailBoxList(mailimf_mailbox_list *mbList)
{
    clistiter *cur;
    QList<Address *> *contacts = new QList<Address *>();

    for (cur = clist_begin(mbList->mb_list); cur != NULL; cur = clist_next(cur)) {
        struct mailimf_mailbox *mb;
        mb = (mailimf_mailbox *)clist_content(cur);
        Address *address = new Address();

        if (mb->mb_display_name != NULL)
            address->setDisplayName(mimeToUtf8(mb->mb_display_name));

        address->setAddress(mimeToUtf8(mb->mb_addr_spec));

        contacts->append(address);
    }

    return contacts;
}

QList<Address *> *Message::parseAddressList(mailimf_address_list *addressList)
{
    clistiter *cur;
    QList<Address *> *addrList = new QList<Address *>();

    for (cur = clist_begin(addressList->ad_list); cur != NULL; cur = clist_next(cur)) {
        mailimf_address *addr = (mailimf_address *)clist_content(cur);

        addrList->append(*parseAddress(addr));
    }

    return addrList;
}

QList<Address *> *Message::parseAddress(mailimf_address *addr)
{
    QList<Address *> *addresses = new QList<Address *>();

    switch (addr->ad_type) {
    case MAILIMF_ADDRESS_GROUP: {
        clistiter *it;

        for (it = clist_begin(addr->ad_data.ad_group->grp_mb_list->mb_list); it != NULL; it = clist_next(it)) {
            struct mailimf_mailbox *mb;

            mb = (mailimf_mailbox *)clist_content(it);
            Address *address = new Address();
            if (mb->mb_display_name != NULL) {
                address->setDisplayName(mimeToUtf8(mb->mb_display_name));
            }
            address->setAddress(mimeToUtf8(mb->mb_addr_spec));
            addresses->append(address);
        }
    } break;

    case MAILIMF_ADDRESS_MAILBOX: {
        struct mailimf_mailbox *mb;
        mb = addr->ad_data.ad_mailbox;
        Address *address = new Address();
        if (mb->mb_display_name != NULL) {
            address->setDisplayName(mimeToUtf8(mb->mb_display_name));
        }

        address->setAddress(mimeToUtf8(mb->mb_addr_spec));
        addresses->append(address);
    } break;
    }

    return addresses;
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

void Message::parseMime(mailmime *mime)
{
    QString mimeType = getMimeType(mime->mm_content_type);

    switch (mime->mm_type) {
    case MAILMIME_SINGLE:
        parseSingleMime(mime, mimeType);
        break;

    case MAILMIME_MULTIPLE:
        for (clistiter *cur = clist_begin(mime->mm_data.mm_multipart.mm_mp_list); cur != NULL; cur = clist_next(cur)) {
            parseMime((mailmime *)clist_content(cur));
        }
        break;
    case MAILMIME_MESSAGE:
        if (mime->mm_data.mm_message.mm_fields) {
            parseHeaderFields(mime->mm_data.mm_message.mm_fields);
        }
        if (mime->mm_data.mm_message.mm_msg_mime != NULL) {
            parseMime(mime->mm_data.mm_message.mm_msg_mime);
        }

        break;
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
        int rv = mailmime_part_parse(data->dt_data.dt_text.dt_data, data->dt_data.dt_text.dt_length, &index, data->dt_encoding, &parsedStr, &parsedLen);

        if (rv == MAILIMF_NO_ERROR) {
            MessagePart *bodypart = new MessagePart();
            bodypart->setCharset(charset);
            bodypart->setMimeType(mimeType);
            bodypart->setSize(std::string(parsedStr, parsedLen).size());

            if (isAttachment) {
                bodypart->setIsAttachment(isAttachment);
                bodypart->setPayload(QByteArray(parsedStr, parsedLen));
                bodypart->setFilename(fileName);
            } else {
                if (mimeType == "text/html") {
                    bodypart->setTextHtml(QString::fromStdString(parsedStr));
                } else if (mimeType == "text/plain") {
                    bodypart->setPlainText(QString::fromStdString(parsedStr));
                }
            }
            m_messageParts.append(bodypart);
        }

    } break;

    case MAILMIME_DATA_FILE:
        break;

    default:
        break;
    }
}

void Message::parseMimeFields(mailmime *mime, QString &filename, QString &charset, bool &isattachment)
{
    mailmime_single_fields fields;
    memset(&fields, 0, sizeof(mailmime_single_fields));
    if (mime->mm_mime_fields != NULL) {
        mailmime_single_fields_init(&fields, mime->mm_mime_fields, mime->mm_content_type);

        if (fields.fld_disposition != NULL) {
            mailmime_disposition_type *type = fields.fld_disposition->dsp_type;
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

QString Message::mimeToUtf8(std::string str)
{
    const char *charset = "UTF-8";
    char *cdecoded = NULL;
    size_t curtoken = 0;
    int rv = mailmime_encoded_phrase_parse(charset, str.c_str(), str.size(), &curtoken, charset, &cdecoded);
    if ((rv == MAILIMF_NO_ERROR) && (cdecoded != NULL)) {
        std::string decoded(cdecoded);
        free(cdecoded);
        return QString::fromStdString(decoded);
    } else {
        return QString::fromStdString(str);
    }
}

MessageHeader *Message::header() const
{
    return m_header;
}

const QList<MessagePart *> &Message::messageParts() const
{
    return m_messageParts;
}

void Message::setMessageParts(const QList<MessagePart *> &newMessageParts)
{
    m_messageParts = newMessageParts;
}

QString Message::html()
{
}

const QString &Message::accountEmail() const
{
    return m_accountEmail;
}

void Message::setAccountEmail(const QString &newAccountEmail)
{
    m_accountEmail = newAccountEmail;
}

ssize_t Message::uid() const
{
    return m_uid;
}

void Message::setUid(ssize_t newUid)
{
    m_uid = newUid;
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
