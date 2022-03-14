#include "messagepart.h"

MessagePart::MessagePart(QObject *parent)
    : QObject{parent}
{

}

const QString &MessagePart::charset() const
{
    return m_charset;
}

void MessagePart::setCharset(const QString &newCharset)
{
    m_charset = newCharset;
}

bool MessagePart::isAttachment() const
{
    return m_isAttachment;
}

void MessagePart::setIsAttachment(bool newIsAttachment)
{
    m_isAttachment = newIsAttachment;
}

const QByteArray &MessagePart::payload() const
{
    return m_payload;
}

void MessagePart::setPayload(const QByteArray &newPayload)
{
    m_payload = newPayload;
}

const QString &MessagePart::filename() const
{
    return m_filename;
}

void MessagePart::setFilename(const QString &newFilename)
{
    m_filename = newFilename;
}

const QString &MessagePart::mimeType() const
{
    return m_mimeType;
}

void MessagePart::setMimeType(const QString &newMimeType)
{
    m_mimeType = newMimeType;
}

int MessagePart::size() const
{
    return m_size;
}

void MessagePart::setSize(int newSize)
{
    m_size = newSize;
}

const QString &MessagePart::textHtml() const
{
    return m_textHtml;
}

void MessagePart::setTextHtml(const QString &newTextHtml)
{
    m_textHtml = newTextHtml;
}

const QString &MessagePart::plainText() const
{
    return m_plainText;
}

void MessagePart::setPlainText(const QString &newPlainText)
{
    m_plainText = newPlainText;
}
