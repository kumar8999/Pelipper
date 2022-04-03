#include "syncworker.h"

#include <QDebug>
#include <QSet>

SyncWorker::SyncWorker(QObject *parent)
    : QObject(parent)
{
    m_account = nullptr;
    m_imapService = nullptr;
}

void SyncWorker::loadCacheFolders()
{
    ImapCache *imapCache = m_account->cacheService();
    QList<Folder *> *folderList = imapCache->getFolders();

    emit foldersReadFinished(folderList);
}

void SyncWorker::fetchFolders()
{
    QList<Folder *> *folderList = m_imapService->getFolders();
    emit foldersReadFinished(folderList);
}

void SyncWorker::fetchCacheFolders()
{
    loadCacheFolders();
    fetchFolders();
}

void SyncWorker::setAccount(Account *newAccount)
{
    m_account = newAccount;
    m_imapService = m_account->createImapService();

    m_imapService->connect();
    m_imapService->login();
}

Account *SyncWorker::account() const
{
    return m_account;
}

void SyncWorker::fetchHeaders()
{
    QList<Message *> *messageList = m_account->cacheService()->getAllMessages(selectedFolder);

    if (!messageList->isEmpty()) {
        emit messagesReadFinished(messageList);
    }

    qDebug() << "fetch headers";

    QList<ssize_t> uidList = m_imapService->getUids(selectedFolder);

    QList<ssize_t> cachedUidList = m_account->cacheService()->getUidList(selectedFolder);

    QSet<ssize_t> uidListSet = QSet<ssize_t>(uidList.begin(), uidList.end());
    QSet<ssize_t> cachedUidSet = QSet<ssize_t>(cachedUidList.begin(), cachedUidList.end());

    QSet<ssize_t> newUidListSet = uidListSet - cachedUidSet;

    QList<ssize_t> newUidList = QList<ssize_t>(newUidListSet.begin(), newUidListSet.end());

    messageList = m_imapService->fetchMessage(selectedFolder, newUidList, static_cast<FetchType>(FetchType::Header | FetchType::Flag));

    emit messagesReadFinished(messageList);
}

void SyncWorker::setSelectedFolder(const QString &newSelectedFolder)
{
    selectedFolder = newSelectedFolder;
}

void SyncWorker::syncFolders()
{
    QStringList folderList = m_account->cacheService()->fetchFoldersStr();

    for (const auto &folder : qAsConst(folderList)) {
        qDebug() << "cached folder" << folder;
        QList<ssize_t> uidList = m_account->cacheService()->getUidList(folder);
        qDebug() << uidList;
        emit cachedUidList(uidList);
    }

    folderList = m_imapService->fetchFoldersStringList();
    m_account->cacheService()->insertFoldersStr(folderList);

    for (const auto &folder : qAsConst(folderList)) {
        qDebug() << "syncing folder" << folder;
        QList<ssize_t> uidList = m_imapService->getUids(folder);
        emit fetchedUidList(folder, uidList);

        m_account->cacheService()->insertUidList(folder, uidList);
    }
}

const QString &SyncWorker::getSelectedFolder() const
{
    return selectedFolder;
}
