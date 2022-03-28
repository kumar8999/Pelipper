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

    emit foldersReadFinished(folders);
}

void SyncWorker::loadMessages(const QString &folder, const FetchType &fetchType)
{
    ImapService *imapService = m_account->IMAPService();
    ImapCache *imapCache = m_account->cacheService();
    QList<ssize_t> uidList = imapService->getUids(folder);
    QList<ssize_t> cachedUidList = imapCache->getUidList(folder);

    QSet<ssize_t> uidSet = QSet<ssize_t>(uidList.begin(), uidList.end());
    QSet<ssize_t> cachedUidSet = QSet<ssize_t>(cachedUidList.begin(), cachedUidList.end());
    QSet<ssize_t> nonCachedUidSet = uidSet - cachedUidSet;
    QList<ssize_t> nonCachedUidList = QList<ssize_t>(nonCachedUidSet.begin(), nonCachedUidSet.end());

    QList<Message *> *messageList = imapService->fetchMessage(folder, nonCachedUidList, fetchType);

    emit messagesReadFinished(messageList);
}

void SyncWorker::loadMessage(const QString &folder, const ssize_t &uid)
{
    ImapService *imapService = m_account->IMAPService();
    ImapCache *imapCache = m_account->cacheService();

    QString data;
    Message *msg = imapCache->getMessage(folder, uid, data);

    if (msg == nullptr) {
        QList<ssize_t> uidList;
        uidList.append(uid);
        QList<Message *> *msgList = imapService->fetchMessage(folder, uidList, static_cast<FetchType>(FetchType::Body | FetchType::Flag));
        msg = msgList->first();
    }

    emit messageReadFinished(msg);
}

void SyncWorker::setSelectedFolder(const QString &newSelectedFolder)
{
    m_selectedFolder = newSelectedFolder;
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

void SyncWorker::fetchHeaders()
{
    loadMessages(m_selectedFolder, static_cast<FetchType>(FetchType::Header | FetchType::Flag));
}

void SyncWorker::fetchMessage()
{
    loadMessage(m_selectedFolder, m_uid);
}
