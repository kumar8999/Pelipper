#ifndef MESSAGEPART_H
#define MESSAGEPART_H

#include <QObject>

class MessagePart : public QObject
{
    Q_OBJECT
public:
    explicit MessagePart(QObject *parent = nullptr);

    const QString &charset() const;
    void setCharset(const QString &newCharset);

    bool isAttachment() const;
    void setIsAttachment(bool newIsAttachment);

    const QByteArray &payload() const;
    void setPayload(const QByteArray &newPayload);

    const QString &filename() const;
    void setFilename(const QString &newFilename);

    const QString &mimeType() const;
    void setMimeType(const QString &newMimeType);

    int size() const;
    void setSize(int newSize);

    const QString &textHtml() const;
    void setTextHtml(const QString &newTextHtml);

    const QString &plainText() const;
    void setPlainText(const QString &newPlainText);

signals:

private:
    QString m_textHtml;
    QString m_plainText;
    bool m_isAttachment;
    QByteArray m_payload;
    QString m_filename;
    QString m_charset;
    QString m_mimeType;
    int m_size;
};

#endif // MESSAGEPART_H
