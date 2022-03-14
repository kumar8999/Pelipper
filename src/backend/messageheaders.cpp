#include "messageheaders.h"

#include <QDebug>

MessageHeader::MessageHeader(QObject *parent)
    : QObject{parent}
{
}

Address *MessageHeader::from() const
{
    return m_from;
}

void MessageHeader::setFrom(Address *newFrom)
{
    m_from = newFrom;
}

Address *MessageHeader::sender() const
{
    return m_sender;
}

void MessageHeader::setSender(Address *newSender)
{
    m_sender = newSender;
}

QList<Address *> *MessageHeader::getReplyTo() const
{
    return m_replyTo;
}

void MessageHeader::setReplyTo(QList<Address *> *newReplyTo)
{
    m_replyTo = newReplyTo;
}

QList<Address *> *MessageHeader::cc() const
{
    return m_cc;
}

void MessageHeader::setCc(QList<Address *> *newCc)
{
    m_cc = newCc;
}

QList<Address *> *MessageHeader::bcc() const
{
    return m_bcc;
}

void MessageHeader::setBcc(QList<Address *> *newBcc)
{
    m_bcc = newBcc;
}

const QString &MessageHeader::messageId() const
{
    return m_messageId;
}

void MessageHeader::setMessageId(const QString &newMessageId)
{
    m_messageId = newMessageId;
}

const QString &MessageHeader::subject() const
{
    return m_subject;
}

void MessageHeader::setSubject(const QString &newSubject)
{
    m_subject = newSubject;
}

QList<Address *> *MessageHeader::to() const
{
    return m_to;
}

void MessageHeader::setTo(QList<Address *> *newTo)
{
    m_to = newTo;
}

const QDateTime &MessageHeader::dateTime() const
{
    return m_dateTime;
}

void MessageHeader::setDateTime(const QDateTime &newDateTime)
{
    m_dateTime = QDateTime();
    m_dateTime.setTime(newDateTime.time());
    m_dateTime.setDate(newDateTime.date());
}
