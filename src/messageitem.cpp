#include "messageitem.h"

#include <QDebug>

MessageItem::MessageItem(QObject *parent)
    : QObject(parent)
{
}

const QString &MessageItem::content() const
{
    return m_content;
}

const QString &MessageItem::to() const
{
    return m_to;
}

const QString &MessageItem::from() const
{
    return m_from;
}

const QString &MessageItem::subject() const
{
    return m_subject;
}

bool MessageItem::hasAttachment() const
{
    return m_hasAttachment;
}

const QDateTime &MessageItem::datetime() const
{
    return m_datetime;
}

void MessageItem::setMessage(Message *newMessage)
{
    m_Message = newMessage;
    m_datetime = m_Message->DateTime();
    m_subject = m_Message->Subject();

    m_from = "";
    m_to = "";

    qDebug() << "contacts" << m_Message->From()->Hostname().isEmpty() << m_Message->From()->Hostname();

    if (!m_Message->From()->Hostname().isEmpty()) {
        m_from += m_Message->From()->Hostname();
        m_from += " <" + m_Message->From()->Address() + ">";
    } else {
        m_from += m_Message->From()->Address();
    }

    for (auto contact : m_Message->To()) {
        if (!contact->Hostname().isEmpty()) {
            m_to += contact->Hostname();
            m_to += " <" + contact->Address() + ">";
        } else {
            m_to += contact->Address();
        }
    }

    if (m_Message->Html().isEmpty()) {
        m_content = m_Message->PlainText();
    } else {
        m_content = m_Message->Html();
    }
}
