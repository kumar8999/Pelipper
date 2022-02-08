#ifndef SMTPSERVICE_H
#define SMTPSERVICE_H

#include "contact.h"

#include <libetpan/libetpan.h>

#include <QMutex>
#include <QObject>

class SmtpService : public QObject
{
public:
    SmtpService(const QString &user,
                const QString &password,
                const QString &server,
                const int port,
                const QString &email,
                const int timeout,
                QObject *parent = nullptr);

    virtual ~SmtpService();

    bool Send(const QString &subject,
              const QString &message,
              const QString &htmlMessage,
              const QList<Contact> &to,
              const QList<Contact> &cc,
              const QList<Contact> &bcc,
              const QString &refMsgId,
              const QList<QString> &attachmentPaths,
              const bool flowed,
              QString &resultMessage);

    bool Send(const QString &data, const QList<Contact> &to, const QList<Contact> &cc, const QList<Contact> &bcc);
    QString getHeader(const QString &subject, const QList<Contact> &to, const QList<Contact> &cc, const QList<Contact> &bcc, const QString &refMsgId);
    QString getBody(const QString &message, const QString &htmlMessage, const QList<QString> &attachmentPaths, bool flowed);

private:
    bool SendMessage(const QString &data, const QList<Contact> &recipients);
    struct mailmime *GetMimeTextPart(const char *mimeType, const QString &message, bool flowed);
    struct mailmime *GetMimeFilePart(const QString &path, const QString &mimeType = "application/octet-stream");
    struct mailmime *GetMimePart(struct mailmime_content *p_Content, struct mailmime_fields *p_MimeFields, bool p_ForceSingle);
    QString MimeEncodeStr(const QString &p_In);

private:
    QMutex m_Mutex;
    QString m_User;
    QString m_Password;
    QString m_Server;
    uint16_t m_Port;
    QString m_Email;
    int m_Timeout = 0;
};

#endif // SMTPSERVICE_H
