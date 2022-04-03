#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "folder.h"
#include "idlemanager.h"
#include "imapcache.h"
#include "imapservice.h"
#include "smtpservice.h"

#include <QObject>

class Account : public QObject
{
    Q_OBJECT
public:
    explicit Account(const QString &username,
                     const QString &email,
                     const QString &password,
                     const QString &imapServer,
                     const int &imapPort,
                     const QString &smtpServer,
                     const int &smtpPort,
                     QObject *parent = nullptr);

    ~Account();

    const QString &Email() const;

    SmtpService *SMTPService() const;

    ImapCache *cacheService() const;

    ImapService *createImapService();

private:
    QString m_Username;
    QString m_email;
    QString m_password;
    QString m_imapServer;
    QString m_SmtpServer;
    int m_imapPort;
    int m_SmtpPort;

    QList<ImapService *> m_imapServices;

    SmtpService *m_SmtpService;
    ImapCache *m_cacheService;
};

#endif // ACCOUNT_H
