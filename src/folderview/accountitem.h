#ifndef ACCOUNTITEM_H
#define ACCOUNTITEM_H

#include "../backend/account.h"
#include "../backend/folder.h"
#include "folderitem.h"

#include <QObject>
#include <QStandardItem>

class AccountItem : public QObject, public QStandardItem
{
    Q_OBJECT
public:
    explicit AccountItem(Account *account);

    Account *account() const;

    const QString &email() const;

    void clearAll();

private:
    QString m_email;
    Account *m_account;
};

#endif // ACCOUNTITEM_H
