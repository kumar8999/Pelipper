#ifndef SYNCMANAGER_H
#define SYNCMANAGER_H

#include "account.h"
#include "session.h"
#include "settings.h"

#include <QMap>
#include <QObject>

class SyncWorker;

class SyncManager : public QObject
{
    Q_OBJECT
public:
    explicit SyncManager(QObject *parent = nullptr);

    void loadCache(Account *account);

    bool isCacheThreadRunning();

    void fetchHeaders(QHash<Account *, QString> *accountFolder);

    void fetchHeaders(Account *account, QString folder);

    void startSync();

    void startCache();

signals:
    void foldersReadFinished(Account *account, QList<Folder *> *folderList);

    void messagesReadFinished(Account *account, const QString &folder, QList<Message *> *msgList);

    void newUidList(QString email, QString folder, QList<ssize_t> uidList);

private slots:
    void onWorkerFoldersReadFinished(QList<Folder *> *folderList);

    void onCachedUidListReadFinished(const QList<ssize_t> &uidList);

    void onFetchedUidListReadFinished(QString folder, const QList<ssize_t> &uidList);

    void onWorkerMessagesReadFinished(QList<Message *> *msgList);

private:
    void cancelAllMessageThread();

private:
    QMap<QString, QThread *> m_cacheThread;
    QMap<QString, QThread *> m_messagesThread;

    QMap<QString, QList<ssize_t>> m_uidList;
};

#endif // SYNCMANAGER_H
