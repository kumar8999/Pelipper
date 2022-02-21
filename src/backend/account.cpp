#include "account.h"

#include <QtConcurrent>

Account::Account(const QString &username,
                 const QString &email,
                 const QString &password,
                 const QString &imapServer,
                 const int &imapPort,
                 const QString &smtpServer,
                 const int &smtpPort,
                 QObject *parent)
    : m_Username(username)
    , m_Email(email)
    , m_Password(password)
    , m_ImapServer(imapServer)
    , m_ImapPort(imapPort)
    , m_SmtpServer(smtpServer)
    , m_SmtpPort(smtpPort)
    , QObject(parent)
{
    m_ImapService = new ImapService(email,
                                    password,
                                    imapServer,
                                    imapPort,
                                    ConnectionType::ConnectionSSL);
    m_SmtpService
        = new SmtpService(m_Username, m_Password, m_SmtpServer, m_SmtpPort, m_Email, 0, this);
    m_IdleManager = new IdleManager(new ImapService(m_Email,
                                                    m_Password,
                                                    m_ImapServer,
                                                    m_ImapPort,
                                                    ConnectionType::ConnectionSSL));
}

Account::~Account()
{
    delete m_ImapService;
    delete m_SmtpService;
}

ImapService *Account::IMAPService() const
{
    return m_ImapService;
}

const QString &Account::Email() const
{
    return m_Email;
}

IdleManager *Account::idleManager() const
{
    return m_IdleManager;
}

SmtpService *Account::SMTPService() const
{
    return m_SmtpService;
}

void Account::startCacheService()
{
    //    qDebug() << m_Email << m_Password;
    //    ImapService *imapService = new ImapService(m_Email, m_Password, m_ImapServer, m_ImapPort);
    //    qDebug() << imapService->connect();
    //    qDebug() << imapService->login();

    //    imapService->startCache();
}
