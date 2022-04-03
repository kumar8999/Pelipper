#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include "syncmanager.h"

#include <QObject>

enum MessageOperation { MoveOperation, DeleteOperation };

class MessageHandler : public QObject
{
    Q_OBJECT
public:
    explicit MessageHandler(QObject *parent = nullptr);

    SyncManager *syncmanager() const;
    void setSyncmanager(SyncManager *newSyncmanager);

    void fetchHeaders(QHash<Account *, QString> *accountFolder);

    QList<Message *> *messages() const;

signals:
    void messageLoadFinished();

private slots:
    void onMessageReadFinished(Account *account, const QString &folder, QList<Message *> *msgList);

private:
    SyncManager *m_syncmanager;

    QList<Message *> *m_messages;
};

#endif // MESSAGEHANDLER_H
