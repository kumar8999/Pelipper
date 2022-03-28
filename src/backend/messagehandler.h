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

    void deleteMessages();

    void fetchHeaders(QHash<Account *, Folder *> *accountFolder);

    void fetchMessage(Account *account, const QString &foldername, const ssize_t &uid);

    void setSyncmanager(SyncManager *newSyncmanager);

    void clearAllMessages();

    const QMap<QString, QList<Message *> *> &messages() const;

signals:

    void messageLoadFinished();

private slots:
    void onMessagesReadFinished(QMap<QString, QList<Message *> *> messages);

private:
    SyncManager *m_syncmanager;

    QMap<QString, QList<Message *> *> m_messages;
};

#endif // MESSAGEHANDLER_H
