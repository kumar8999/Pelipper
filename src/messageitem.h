#ifndef MESSAGEITEM_H
#define MESSAGEITEM_H

#include "message.h"

#include <QObject>

class MessageItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString content READ content NOTIFY contentChanged)
    Q_PROPERTY(QString to READ to NOTIFY toChanged)
    Q_PROPERTY(QString from READ from NOTIFY fromChanged)
    Q_PROPERTY(QString subject READ subject NOTIFY subjectChanged)
    Q_PROPERTY(bool hasAttachment READ hasAttachment NOTIFY hasAttachmentChanged)
    Q_PROPERTY(QDateTime datetime READ datetime NOTIFY datetimeChanged)

public:
    explicit MessageItem(QObject *parent = nullptr);

    const QString &content() const;

    const QString &to() const;

    const QString &from() const;

    const QString &subject() const;

    bool hasAttachment() const;

    const QDateTime &datetime() const;

    void setMessage(Message *newMessage);

signals:
    void contentChanged();
    void toChanged();
    void fromChanged();
    void subjectChanged();
    void hasAttachmentChanged();
    void datetimeChanged();

private:
    QString m_content;
    QString m_to;
    QString m_from;
    QString m_subject;
    bool m_hasAttachment;
    QDateTime m_datetime;

    Message *m_Message;
};

#endif // MESSAGEITEM_H
