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

    void fetchHeaders();

signals:
    void foldersReadFinished(Account *account, QList<Folder *> *folders);

    void messagesReadFinished(QMap<QString, QList<Message *> *> messages);

private slots:
    void onAccountAdded(Account *account);

    void onWorkerFolderReadFinished(QList<Folder *> *folders);

private:
    QMap<QThread *, SyncWorker *> m_thread;

    QMap<QString, QThread *> m_folderThread;

    QList<Account *> m_accounts;
};

#endif // SYNCMANAGER_H
