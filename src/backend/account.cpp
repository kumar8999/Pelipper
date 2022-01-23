#include "account.h"

#include <QDebug>
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
    m_ImapService = new ImapService(email, password, imapServer, imapPort);
}

Account::~Account()
{
    delete m_ImapService;
}

ImapService *Account::IMAPService() const
{
    return m_ImapService;
}

const QString &Account::Email() const
{
    return m_Email;
}
