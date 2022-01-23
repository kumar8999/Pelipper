#ifndef SESSION_H
#define SESSION_H

#include "account.h"

#include <QMap>
#include <QObject>

class Session : public QObject
{
    Q_OBJECT

public:
    Session(QObject *parent = nullptr);

    static Session *getInstance();

    QList<Account *> getAccounts();

    Account *getAccount(QString email);

    void addAccount(Account *account);

    void removeAccount(Account *account);

signals:
    void accountAdded(Account *account);

    void accountRemoved(Account *account);

private:
    static Session *m_Session;
    QMap<QString, Account *> *m_Accounts;
};

#endif // SESSION_H
