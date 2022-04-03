#ifndef SMTPSERVICE_H
#define SMTPSERVICE_H

#include "address.h"

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
              const QList<Address> &to,
              const QList<Address> &cc,
              const QList<Address> &bcc,
              const QStringList &attachmentPaths);

private:
    bool SendMessage(const QString &data, const QList<Address> &recipientList);

    mailimf_address_list *createAddressField(QList<Address> contactList);

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
