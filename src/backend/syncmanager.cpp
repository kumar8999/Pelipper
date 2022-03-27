#include "syncmanager.h"
#include "syncworker.h"

#include <QDebug>
#include <QSet>
#include <QThread>

SyncManager::SyncManager(QObject *parent)
    : QObject{parent}
{
    auto *session = Session::getInstance();
    m_accounts = session->getAccounts();
    connect(session, &Session::accountAdded, this, &SyncManager::onAccountAdded);
}

bool SyncManager::isFolderThreadRunning()
{
    QMapIterator<QString, QThread *> mapIter(m_folderThread);

    while (mapIter.hasNext()) {
        mapIter.next();

        if (mapIter.value()->isRunning()) {
            return true;
        }
    }

    return false;
}

void SyncManager::onAccountAdded(Account *account)
{
    QThread *thread = m_folderThread[account->Email()];

    if (thread == nullptr) {
        thread = new QThread();
        m_folderThread[account->Email()] = thread;
    }

    SyncWorker *worker = new SyncWorker();
    worker->setAccount(account);

    connect(thread, &QThread::started, worker, &SyncWorker::fetchFolders);
    connect(worker, &SyncWorker::foldersReadFinished, this, &SyncManager::onWorkerFolderReadFinished);

    thread->start();
}

void SyncManager::onWorkerFolderReadFinished(QList<Folder *> *folders)
{
    SyncWorker *worker = static_cast<SyncWorker *>(sender());
    emit foldersReadFinished(worker->account(), folders);
}
