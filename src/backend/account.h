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

    ImapService *IMAPService() const;

    const QString &Email() const;

    IdleManager *idleManager() const;

    SmtpService *SMTPService() const;

    ImapService *idleService() const;

    ImapService *createIdleService();

    ImapCache *cacheService() const;

private:
    QString m_Username;
    QString m_Email;
    QString m_Password;
    QString m_ImapServer;
    QString m_SmtpServer;
    int m_ImapPort;
    int m_SmtpPort;

    ImapService *m_ImapService;
    ImapService *m_idleService;
    IdleManager *m_IdleManager;
    SmtpService *m_SmtpService;
    ImapCache *m_cacheService;
};

#endif // ACCOUNT_H
