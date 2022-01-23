#include "messageitem.h"

MessageItem::MessageItem(Message *msg, QObject *parent)
    : m_Message(msg)
    , QObject{parent}
{
}

QString MessageItem::cc()
{
    if (m_Message == nullptr)
        return "";

    QList<Contact *> contacts = m_Message->cc();

    if (contacts.length() == 1) {
        if (contacts.at(0)->Hostname() == "") {
            return contacts.at(0)->Address();
        } else {
            return contacts.at(0)->Hostname() + "<" + contacts.at(0)->Address() + ">";
        }
    }

    QString cc;

    for (int i = 0; i < contacts.length(); i++) {
        if (contacts.at(0)->Hostname() == "") {
            cc += contacts.at(0)->Address() + ",";
        } else {
            cc += contacts.at(0)->Hostname() + "<" + contacts.at(0)->Address() + ">,";
        }
    }

    return cc;
}

QString MessageItem::bcc()
{
    if (m_Message == nullptr)
        return "";

    QList<Contact *> contacts = m_Message->Bcc();

    if (contacts.length() == 1) {
        if (contacts.at(0)->Hostname() == "") {
            return contacts.at(0)->Address();
        } else {
            return contacts.at(0)->Hostname() + "<" + contacts.at(0)->Address() + ">";
        }
    }

    QString bcc;

    for (int i = 0; i < contacts.length(); i++) {
        if (contacts.at(0)->Hostname() == "") {
            bcc += contacts.at(0)->Address() + ",";
        } else {
            bcc += contacts.at(0)->Hostname() + "<" + contacts.at(0)->Address() + ">,";
        }
    }

    return bcc;
}

QString MessageItem::from()
{
    if (m_Message == nullptr)
        return "";

    Contact *contact = m_Message->From();

    if (contact->Hostname() == "") {
        return contact->Address();
    } else {
        return contact->Hostname() + "<" + contact->Address() + ">";
    }
}

QString MessageItem::to()
{
    if (m_Message == nullptr)
        return "";

    QList<Contact *> contacts = m_Message->To();

    if (contacts.length() == 1) {
        if (contacts.at(0)->Hostname() == "") {
            return contacts.at(0)->Address();
        } else {
            return contacts.at(0)->Hostname() + "<" + contacts.at(0)->Address() + ">";
        }
    }

    QString to;

    for (int i = 0; i < contacts.length(); i++) {
        if (contacts.at(0)->Hostname() == "") {
            to += contacts.at(0)->Address() + ",";
        } else {
            to += contacts.at(0)->Hostname() + "<" + contacts.at(0)->Address() + ">,";
        }
    }

    return to;
}

QString MessageItem::subject()
{
    if (m_Message == nullptr)
        return "";

    return m_Message->Subject();
}

int MessageItem::uid()
{
    if (m_Message == nullptr)
        return 0;

    return m_Message->Uid();
}

QString MessageItem::html()
{
    if (m_Message == nullptr)
        return "";

    if (m_Message->Html().isEmpty()) {
        return m_Message->PlainText();
    }
    return m_Message->Html();
}

QDateTime MessageItem::datetime()
{
    if (m_Message == nullptr)
        return QDateTime();

    return m_Message->DateTime();
}

bool MessageItem::hasAttachments()
{
    if (m_Message == nullptr)
        return false;

    return m_Message->hasAttachment();
}

void MessageItem::setMessage(Message *newMessage)
{
    m_Message = newMessage;
}

void MessageItem::setSubject(const QString &newSubject)
{
    if (m_subject == newSubject)
        return;
    m_subject = newSubject;
    emit subjectChanged();
}

void MessageItem::setHtml(const QString &newHtml)
{
    if (m_html == newHtml)
        return;
    m_html = newHtml;
    emit htmlChanged();
}

void MessageItem::setDatetime(const QDateTime &newDatetime)
{
    if (m_datetime == newDatetime)
        return;
    m_datetime = newDatetime;
    emit datetimeChanged();
}

void MessageItem::setHasAttachments(bool newHasAttachments)
{
    if (m_hasAttachments == newHasAttachments)
        return;
    m_hasAttachments = newHasAttachments;
    emit hasAttachmentsChanged();
}

void MessageItem::setFrom(const QString &newFrom)
{
    if (m_from == newFrom)
        return;
    m_from = newFrom;
    emit fromChanged();
}

void MessageItem::setTo(const QString &newTo)
{
    if (m_to == newTo)
        return;
    m_to = newTo;
    emit toChanged();
}

void MessageItem::setBcc(const QString &newBcc)
{
    if (m_bcc == newBcc)
        return;
    m_bcc = newBcc;
    emit bccChanged();
}

void MessageItem::setCc(const QString &newCc)
{
    if (m_cc == newCc)
        return;
    m_cc = newCc;
    emit ccChanged();
}
