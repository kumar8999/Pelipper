#ifndef MESSAGE_H
#define MESSAGE_H

#include "address.h"
#include "contact.h"
#include "flags.h"
#include "messageheaders.h"
#include "messagepart.h"

#include <libetpan/libetpan.h>

#include <QDateTime>
#include <QObject>

class Message : public QObject
{
    Q_OBJECT

public:
    Message(const QString &accountEmail, ssize_t uid, QObject *parent = nullptr);

    void parseFromData(const QString &data);

    const QString &Data() const;

    Flags *flags() const;
    void setFlags(Flags *newFlags);

    ssize_t uid() const;
    void setUid(ssize_t newUid);

    const QString &accountEmail() const;
    void setAccountEmail(const QString &newAccountEmail);

    QString html();

    const QList<MessagePart *> &messageParts() const;
    void setMessageParts(const QList<MessagePart *> &newMessageParts);

    MessageHeader *header() const;

private:
    void parseHeaderFields(mailimf_fields *headerFields);
    Address *parseFrom(mailimf_from *mbList);
    Address *parseSender(mailimf_mailbox *mb);
    QList<Address *> *parseTo(mailimf_to *mbList);
    QList<Address *> *parseMailBoxList(mailimf_mailbox_list *mbList);
    QList<Address *> *parseAddressList(mailimf_address_list *addressList);
    QList<Address *> *parseAddress(mailimf_address *addr);

    QString getMimeType(mailmime_content *content);
    QString getDiscreteMimeType(mailmime_discrete_type *type);
    QString getCompositeMimeType(mailmime_composite_type *type);

    void parseData();
    void parseMime(mailmime *mime);
    void parseSingleMime(mailmime *mime, const QString &mimeType);
    void parseMimeFields(mailmime *mime, QString &filename, QString &charset, bool &isattachment);

    QString mimeToUtf8(std::string str);

private:
    QString m_accountEmail;
    ssize_t m_uid;
    QString m_data;

    MessageHeader *m_header;
    Flags *m_flags;

    QList<MessagePart *> m_messageParts;
};
#endif // MESSAGE_H
