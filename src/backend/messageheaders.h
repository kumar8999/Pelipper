#ifndef MESSAGEHEADERS_H
#define MESSAGEHEADERS_H

#include "address.h"

#include <libetpan/libetpan.h>

#include <QDateTime>
#include <QObject>

class MessageHeader : public QObject
{
    Q_OBJECT
public:
    explicit MessageHeader(QObject *parent = nullptr);

    Address *from() const;
    void setFrom(Address *newFrom);

    Address *sender() const;
    void setSender(Address *newSender);

    QList<Address *> *getReplyTo() const;
    void setReplyTo(QList<Address *> *newReplyTo);

    QList<Address *> *cc() const;
    void setCc(QList<Address *> *newCc);

    QList<Address *> *bcc() const;
    void setBcc(QList<Address *> *newBcc);

    const QString &messageId() const;
    void setMessageId(const QString &newMessageId);

    const QString &subject() const;
    void setSubject(const QString &newSubject);

    QList<Address *> *to() const;
    void setTo(QList<Address *> *newTo);

    const QDateTime &dateTime() const;
    void setDateTime(const QDateTime &newDateTime);

private:
    QDateTime m_dateTime;
    Address *m_from;
    Address *m_sender;
    QList<Address *> *m_replyTo;
    QList<Address *> *m_to;
    QList<Address *> *m_cc;
    QList<Address *> *m_bcc;
    QString m_messageId;
    QString m_subject;
};

#endif // MESSAGEHEADERS_H
