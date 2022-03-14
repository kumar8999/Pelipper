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

    QList<Address *> *addresses = m_Message->header()->cc();

    if (addresses->length() == 1) {
        if (addresses->at(0)->displayName() == "") {
            return addresses->at(0)->address();
        } else {
            return addresses->at(0)->displayName() + "<" + addresses->at(0)->address() + ">";
        }
    }

    QString cc;

    for (int i = 0; i < addresses->length(); i++) {
        if (addresses->at(0)->displayName() == "") {
            cc += addresses->at(0)->address() + ",";
        } else {
            cc += addresses->at(0)->displayName() + "<" + addresses->at(0)->address() + ">,";
        }
    }

    return cc;
}

QString MessageItem::bcc()
{
    if (m_Message == nullptr)
        return "";

    QList<Address *> *addresses = m_Message->header()->bcc();

    if (addresses->length() == 1) {
        if (addresses->at(0)->displayName() == "") {
            return addresses->at(0)->address();
        } else {
            return addresses->at(0)->displayName() + "<" + addresses->at(0)->address() + ">";
        }
    }

    QString bcc;

    for (int i = 0; i < addresses->length(); i++) {
        if (addresses->at(0)->displayName() == "") {
            bcc += addresses->at(0)->address() + ",";
        } else {
            bcc += addresses->at(0)->displayName() + "<" + addresses->at(0)->address() + ">,";
        }
    }

    return bcc;
}

QString MessageItem::from()
{
    if (m_Message == nullptr)
        return "";

    Address *address = m_Message->header()->from();

    if (address->displayName() == "") {
        return address->address();
    } else {
        return address->displayName() + "<" + address->address() + ">";
    }
}

QString MessageItem::to()
{
    if (m_Message == nullptr)
        return "";

    QList<Address *> *address = m_Message->header()->to();

    if (address->length() == 1) {
        if (address->at(0)->displayName() == "") {
            return address->at(0)->address();
        } else {
            return address->at(0)->displayName() + "<" + address->at(0)->address() + ">";
        }
    }

    QString to;

    for (int i = 0; i < address->length(); i++) {
        if (address->at(0)->displayName() == "") {
            to += address->at(0)->address() + ",";
        } else {
            to += address->at(0)->displayName() + "<" + address->at(0)->address() + ">,";
        }
    }

    return to;
}

QString MessageItem::subject()
{
    if (m_Message == nullptr)
        return "";

    return m_Message->header()->subject();
}

QString MessageItem::html()
{
    if (m_Message == nullptr)
        return "";

    QList<MessagePart *> messageParts = m_Message->messageParts();

    QString html = "";

    for (auto messagePart : messageParts) {
        html.append(messagePart->textHtml());
    }

    if (!html.isEmpty()) {
        return html;
    }

    for (auto messagePart : messageParts) {
        html.append(messagePart->plainText());
    }

    return html;
}

QDateTime MessageItem::datetime()
{
    if (m_Message == nullptr)
        return QDateTime();

    return m_Message->header()->dateTime();
}

bool MessageItem::hasAttachments()
{
    if (m_Message == nullptr)
        return false;

    QList<MessagePart *> messageParts = m_Message->messageParts();

    for (auto messagePart : messageParts) {
        if (messagePart->isAttachment()) {
            return true;
        }
    }
    return false;
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
