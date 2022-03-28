#ifndef SYNCMANAGER_H
#define SYNCMANAGER_H

#include "account.h"
#include "session.h"

#include <QMap>
#include <QObject>

class SyncWorker;

class SyncManager : public QObject
{
    Q_OBJECT
public:
    explicit SyncManager(QObject *parent = nullptr);

    bool isFolderThreadRunning();

    void fetchHeaders(QHash<Account *, Folder *> *accountFolder);

    void fetchMessages(Account *account, const QString &foldername, const ssize_t &uid);

signals:
    void foldersReadFinished(Account *account, QList<Folder *> *folders);

    void messagesReadFinished(QMap<QString, QList<Message *> *> messages);

    void messageReadFinished(Account *account, Message *message);

private slots:
    void onAccountAdded(Account *account);

    void onWorkerFolderReadFinished(QList<Folder *> *folders);

    void onWorkerMessagesReadFinished(QList<Message *> *messages);

    void onWorkerMessageReadFinished(Message *msg);

private:
    QThread *m_messageThread;

    QMap<QString, QThread *> m_folderThread;
    QMap<QString, QThread *> m_messagesThread;

    QList<Account *> m_accounts;
};

#endif // SYNCMANAGER_H
