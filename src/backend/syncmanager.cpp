#include "syncmanager.h"
#include "syncworker.h"

#include <QDebug>
#include <QSet>
#include <QThread>

SyncManager::SyncManager(QObject *parent)
    : QObject(parent)
{
}

void SyncManager::loadCache(Account *account)
{
    SyncWorker *worker = new SyncWorker(this);
    QThread *thread = new QThread(this);

    m_cacheThread[account->Email()] = thread;

    worker->setAccount(account);
    connect(thread, &QThread::started, worker, &SyncWorker::fetchCacheFolders);
    connect(worker, &SyncWorker::foldersReadFinished, this, &SyncManager::onWorkerFoldersReadFinished);

    thread->start();
}

bool SyncManager::isCacheThreadRunning()
{
    QMapIterator<QString, QThread *> iter(m_cacheThread);

    while (iter.hasNext()) {
        iter.hasNext();

        if (iter.next().value()->isRunning()) {
            return true;
        }
    }

    return false;
}

void SyncManager::fetchHeaders(QHash<Account *, QString> *accountFolder)
{
    cancelAllMessageThread();

    QHashIterator<Account *, QString> iter(*accountFolder);

    while (iter.hasNext()) {
        iter.next();

        m_messagesThread[iter.key()->Email()] = new QThread();

        SyncWorker *worker = new SyncWorker(this);
        worker->setAccount(iter.key());
        worker->setSelectedFolder(iter.value());
        connect(m_messagesThread[iter.key()->Email()], &QThread::started, worker, &SyncWorker::fetchHeaders);
        connect(worker, &SyncWorker::messagesReadFinished, this, &SyncManager::onWorkerMessagesReadFinished);

        m_messagesThread[iter.key()->Email()]->start();
    }
}

void SyncManager::startSync()
{
    QList<Account *> accounts = Session::getInstance()->getAccounts();

    for (auto account : qAsConst(accounts)) {
        SyncWorker *worker = new SyncWorker(this);
        QThread *thread = new QThread(this);
        worker->setAccount(account);
        connect(thread, &QThread::started, worker, &SyncWorker::syncFolders);
        connect(worker, &SyncWorker::cachedUidList, this, &SyncManager::onCachedUidListReadFinished);
        connect(worker, &SyncWorker::fetchedUidList, this, &SyncManager::onFetchedUidListReadFinished);

        thread->start();
    }
}

void SyncManager::startCache()
{
    QList<Account *> accounts = Session::getInstance()->getAccounts();

    for (auto account : qAsConst(accounts)) {
        //        SyncWorker *worker = new SyncWorker(this);
        //        QThread *thread = new QThread(this);
        //        worker->setAccount(account);
        //        connect(thread, &QThread::started, worker, &SyncWorker::syncFolders);
        //        connect(worker, &SyncWorker::cachedUidList, this, &SyncManager::onCachedUidListReadFinished);
        //        connect(worker, &SyncWorker::fetchedUidList, this, &SyncManager::onFetchedUidListReadFinished);

        //        thread->start();
    }
}

void SyncManager::onWorkerFoldersReadFinished(QList<Folder *> *folderList)
{
    SyncWorker *worker = static_cast<SyncWorker *>(sender());
    emit foldersReadFinished(worker->account(), folderList);
}

void SyncManager::onCachedUidListReadFinished(const QList<ssize_t> &uidList)
{
    SyncWorker *worker = static_cast<SyncWorker *>(sender());
    Account *account = worker->account();

    m_uidList[account->Email()] = uidList;
}

void SyncManager::onFetchedUidListReadFinished(QString folder, const QList<ssize_t> &uidList)
{
    SyncWorker *worker = static_cast<SyncWorker *>(sender());
    Account *account = worker->account();

    QList<ssize_t> uids = m_uidList[account->Email()];
    QSet<ssize_t> uidSet = QSet<ssize_t>(uids.begin(), uids.end());
    QSet<ssize_t> uidListSet = QSet<ssize_t>(uidList.begin(), uidList.end());

    QSet<ssize_t> newUid = uidListSet - uidSet;
    emit newUidList(account->Email(), folder, QList<ssize_t>(newUid.begin(), newUid.end()));

    m_uidList[account->Email()] = uidList;
}

void SyncManager::onWorkerMessagesReadFinished(QList<Message *> *msgList)
{
    SyncWorker *worker = static_cast<SyncWorker *>(sender());
    Account *account = worker->account();

    emit messagesReadFinished(account, worker->getSelectedFolder(), msgList);
}

void SyncManager::cancelAllMessageThread()
{
    QMapIterator<QString, QThread *> iter(m_messagesThread);

    while (iter.hasNext()) {
        iter.next();

        if (iter.next().value()->isRunning()) {
            iter.next().value()->quit();
        }
    }
}
