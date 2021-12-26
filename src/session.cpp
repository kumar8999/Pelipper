#include "session.h"
#include <QDebug>

Session *Session::m_Session = nullptr;

Session::Session(QObject *parent)
{
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
    return m_Accounts;
}

void Session::addAccount(Account *account)
{
    m_Accounts.append(account);
    emit accountAdded(account);
}

void Session::removeAccount(Account *account)
{
    for (auto acc : qAsConst(m_Accounts)) {
        if (acc->Email() == account->Email()) {
            m_Accounts.removeAll(acc);
            emit accountRemoved(acc);
            break;
        }
    }
}
