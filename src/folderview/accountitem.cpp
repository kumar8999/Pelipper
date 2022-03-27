#include "accountitem.h"

#include <QtConcurrent>

AccountItem::AccountItem(Account *account)
{
    m_account = account;
    setText(account->Email());
    m_email = account->Email();
}

const QString &AccountItem::email() const
{
    return m_email;
}

void AccountItem::clearAll()
{
    this->removeRows(0, this->rowCount());
}

Account *AccountItem::account() const
{
    return m_account;
}
