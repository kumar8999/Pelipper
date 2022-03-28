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

void SyncManager::fetchHeaders(QHash<Account *, Folder *> *accountFolder)
{
    QHashIterator<Account *, Folder *> accountFolderIter(*accountFolder);

    while (accountFolderIter.hasNext()) {
        accountFolderIter.next();

        Account *account = accountFolderIter.key();
        Folder *folder = accountFolderIter.value();

        QThread *thread = m_messagesThread[account->Email()];

        if (thread != nullptr) {
            thread->quit();
        }

        thread = new QThread();
        m_messagesThread[account->Email()] = thread;

        SyncWorker *worker = new SyncWorker();
        worker->setAccount(account);
        worker->setSelectedFolder(folder->FullName());
        connect(thread, &QThread::started, worker, &SyncWorker::fetchHeaders);
        connect(worker, &SyncWorker::messagesReadFinished, this, &SyncManager::onWorkerMessagesReadFinished);
        thread->start();
    }
}

void SyncManager::fetchMessages(Account *account, const QString &foldername, const ssize_t &uid)
{
    if (m_messageThread != nullptr) {
        m_messageThread->quit();
        m_messageThread = nullptr;
    }

    m_messageThread = new QThread();
    SyncWorker *worker = new SyncWorker();
    worker->setAccount(account);
    worker->setSelectedFolder(foldername);
    connect(m_messageThread, &QThread::started, worker, &SyncWorker::fetchMessage);
    connect(worker, &SyncWorker::messageReadFinished, this, &SyncManager::onWorkerMessageReadFinished);
    m_messageThread->start();
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

void SyncManager::onWorkerMessagesReadFinished(QList<Message *> *messages)
{
    SyncWorker *worker = static_cast<SyncWorker *>(sender());
    QMap<QString, QList<Message *> *> messagesMap;

    messagesMap[worker->account()->Email()] = messages;
    emit messagesReadFinished(messagesMap);
}

void SyncManager::onWorkerMessageReadFinished(Message *msg)
{
    SyncWorker *worker = static_cast<SyncWorker *>(sender());
    emit messageReadFinished(worker->account(), msg);
}
