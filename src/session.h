#ifndef SESSION_H
#define SESSION_H

#include "account.h"
#include "folderlistmodel.h"
#include "messagelistmodel.h"
#include "settings.h"

#include <QObject>

class Session : public QObject
{
    Q_OBJECT

public:
    Session(QObject *parent = nullptr);

    static Session *getInstance();

    QList<Account *> getAccounts();

    void addAccount(Account *account);

    void removeAccount(Account *account);

signals:
    void accountAdded(Account *account);
    void accountRemoved(Account *account);

private:
    void loadSettings();

private:
    static Session *m_Session;
    QList<Account *> m_Accounts;
};

#endif // SESSION_H
