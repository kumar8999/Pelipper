#include "account.h"

#include <QDebug>

Account::Account(const QString &username,
                 const QString &email,
                 const QString &password,
                 const QString &imapServer,
                 const int &imapPort,
                 const QString &smtpServer,
                 const int &smtpPort,
                 QObject *parent)
    : m_Username(username)
    , m_email(email)
    , m_password(password)
    , m_imapServer(imapServer)
    , m_imapPort(imapPort)
    , m_SmtpServer(smtpServer)
    , m_SmtpPort(smtpPort)
    , QObject(parent)
{
    m_SmtpService = new SmtpService(m_Username, m_password, m_SmtpServer, m_SmtpPort, m_email, 0, this);

    m_cacheService = new ImapCache(m_email);
}

Account::~Account()
{
    qDebug() << "deleting account";
}

const QString &Account::Email() const
{
    return m_email;
}

SmtpService *Account::SMTPService() const
{
    return m_SmtpService;
}

ImapCache *Account::cacheService() const
{
    return m_cacheService;
}

ImapService *Account::createImapService()
{
    ImapService *imapService = new ImapService(m_email, m_password, m_imapServer, m_imapPort, ConnectionType::ConnectionSSL);
    //    m_imapServices.append(imapService);
    return imapService;
}
