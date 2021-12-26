#include "accountitem.h"

#include <QtConcurrent>

AccountItem::AccountItem(Account *account)
{
    m_Account = account;
    connect(account, &Account::folderReadFinished, this, &AccountItem::onFoldersReadFinished);
    setText(account->Email());
    loadFolders();
}

void AccountItem::loadFolders()
{
    QtConcurrent::run(m_Account, &Account::getFolders);
}

void AccountItem::onFoldersReadFinished(QList<Folder *> *folderList)
{
    addFolders(this, *folderList, m_Account->Email());
    emit foldersLoadFinished();
}

void AccountItem::addFolders(QStandardItem *parent, QList<Folder *> folders, QString email)
{
    for (auto *folder : folders) {
        FolderItem *childItem = new FolderItem(email, folder->FolderName(), folder->FullFolderName());
        parent->appendRow(childItem);
        if (folder->hasChildren()) {
            addFolders(childItem, folder->Children(), email);
        }
    }
}
