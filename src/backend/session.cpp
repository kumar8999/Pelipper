#include "session.h"

Session *Session::m_Session = nullptr;

Session::Session(QObject *parent)
{
    m_Accounts = new QMap<QString, Account *>();
}

Session *Session::getInstance()
{
    if (m_Session == nullptr) {
        m_Session = new Session();
    }

    return m_Session;
}

QList<Account *> Session::getAccounts()
{
    return m_Accounts->values();
}

Account *Session::getAccount(const QString &email)
{
    if (m_Accounts->contains(email)) {
        return m_Accounts->value(email);
    }

    return nullptr;
}

void Session::addAccount(Account *account)
{
    m_Accounts->insert(account->Email(), account);
    emit accountAdded(account);
}

void Session::removeAccount(Account *account)
{
    if (m_Accounts->contains(account->Email())) {
        m_Accounts->remove(account->Email());
    }
}
