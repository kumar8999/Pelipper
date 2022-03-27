#include "syncworker.h"

#include <QDebug>
#include <QSet>

SyncWorker::SyncWorker(QObject *parent)
    : QObject{parent}
{
    m_account = nullptr;
}

void SyncWorker::loadcacheFolders()
{
    ImapCache *imapCache = m_account->cacheService();

    QList<Folder *> *folders = imapCache->getFolders();

    emit foldersReadFinished(folders);
}

void SyncWorker::loadcacheMessages(const QString &folder)
{
    if (m_account != nullptr) {
        ImapCache *imapCache = m_account->cacheService();
        QList<Message *> *messageList = imapCache->getAllMessages(folder);

        emit messagesReadFinished(messageList);
    }
}

void SyncWorker::loadFolders()
{
    ImapService *imapService = m_account->IMAPService();

    QStringList folderList;

    QList<Folder *> *folders = imapService->getFolders(folderList);

    qDebug() << "folders" << folderList;

    emit foldersReadFinished(folders);
}

void SyncWorker::loadMessages(const QString &folder)
{
    ImapService *imapService = m_account->IMAPService();
    ImapCache *imapCache = m_account->cacheService();
    QList<ssize_t> uidList = imapService->getUids(folder);
    QList<ssize_t> cachedUidList = imapCache->getUidList(folder);

    QSet<ssize_t> uidSet = QSet<ssize_t>(uidList.begin(), uidList.end());
    QSet<ssize_t> cachedUidSet = QSet<ssize_t>(cachedUidList.begin(), cachedUidList.end());
    QSet<ssize_t> nonCachedUidSet = uidSet - cachedUidSet;
    QList<ssize_t> nonCachedUidList = QList<ssize_t>(nonCachedUidSet.begin(), nonCachedUidSet.end());

    FetchType fetchType = static_cast<FetchType>(FetchType::Header | FetchType::Flag);

    QList<Message *> *messageList = imapService->fetchMessage(folder, nonCachedUidList, fetchType);

    emit messagesReadFinished(messageList);
}

Account *SyncWorker::account() const
{
    return m_account;
}

void SyncWorker::setAccount(Account *newAccount)
{
    m_account = newAccount;
}

void SyncWorker::fetchFolders()
{
    loadcacheFolders();
    loadFolders();
}
