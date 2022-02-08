#ifndef INBOXMONITER_H
#define INBOXMONITER_H

#include "account.h"

#include <QObject>

class InboxMoniter : public QObject
{
    Q_OBJECT
public:
    explicit InboxMoniter(QObject *parent = nullptr);

public slots:
    void addAccount(Account *account);

signals:

private:
};

#endif // INBOXMONITER_H
