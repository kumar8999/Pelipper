#ifndef MESSAGEITEM_H
#define MESSAGEITEM_H

#include "../backend/message.h"

#include <QObject>

class MessageItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString cc READ cc WRITE setCc NOTIFY ccChanged)
    Q_PROPERTY(QString bcc READ bcc WRITE setBcc NOTIFY bccChanged)
    Q_PROPERTY(QString from READ from WRITE setFrom NOTIFY fromChanged)
    Q_PROPERTY(QString to READ to WRITE setTo NOTIFY toChanged)
    Q_PROPERTY(QString subject READ subject WRITE setSubject NOTIFY subjectChanged)
    Q_PROPERTY(QString html READ html WRITE setHtml NOTIFY htmlChanged)
    Q_PROPERTY(QDateTime datetime READ datetime WRITE setDatetime NOTIFY datetimeChanged)
    Q_PROPERTY(bool hasAttachments READ hasAttachments WRITE setHasAttachments NOTIFY hasAttachmentsChanged)

public:
    explicit MessageItem(Message *msg, QObject *parent = nullptr);

    void setCc(const QString &newCc);
    QString cc();

    void setBcc(const QString &newBcc);
    QString bcc();

    void setFrom(const QString &newFrom);
    QString from();

    void setTo(const QString &newTo);
    QString to();

    void setSubject(const QString &newSubject);
    QString subject();

    int uid();

    void setHtml(const QString &newHtml);
    QString html();

    void setDatetime(const QDateTime &newDatetime);
    QDateTime datetime();

    void setHasAttachments(bool newHasAttachments);
    bool hasAttachments();

    void setMessage(Message *newMessage);

signals:
    void ccChanged();

    void bccChanged();

    void fromChanged();

    void toChanged();

    void subjectChanged();

    void htmlChanged();

    void datetimeChanged();

    void hasAttachmentsChanged();

private:
    Message *m_Message = nullptr;

    QString m_cc;
    QString m_bcc;
    QString m_from;
    QString m_to;
    QString m_subject;
    QString m_html;
    QDateTime m_datetime;
    bool m_hasAttachments;
};

#endif // MESSAGEITEM_H
