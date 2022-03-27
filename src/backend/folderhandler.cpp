#include "folderhandler.h"

#include <QDebug>

FolderHandler::FolderHandler(QObject *parent)
    : QObject{parent}
{
    m_syncmanager = nullptr;
}

void FolderHandler::setSyncmanager(SyncManager *newSyncmanager)
{
    m_syncmanager = newSyncmanager;

    connect(m_syncmanager, &SyncManager::foldersReadFinished, this, &FolderHandler::onFolderReadFinished);
}

void FolderHandler::onFolderReadFinished(Account *account, QList<Folder *> *folders)
{
    m_folders.insert(account, folders);
    emit folderReadFinished();
}

const QMap<Account *, QList<Folder *> *> &FolderHandler::folders() const
{
    return m_folders;
}
