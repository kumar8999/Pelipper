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

    bool Send(const QString &subject,
              const QString &htmlMessage,
              const QList<Contact> &to,
              const QList<Contact> &cc,
              const QList<Contact> &bcc,
              const QStringList &attachmentPaths);

private:
    bool SendMessage(const QString &data, const QList<Contact> &recipientList);

    mailimf_address_list *createAddressField(QList<Contact> contactList);

    QString getMimeType(const QString &filePath);

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
