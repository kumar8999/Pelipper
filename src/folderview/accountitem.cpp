#include "accountitem.h"

#include <QtConcurrent>

AccountItem::AccountItem(Account *account)
{
    m_account = account;
    setText(account->Email());

    loadFolders();
}

void AccountItem::loadFolders()
{
    QtConcurrent::run(this, &AccountItem::getFolders);
}

void AccountItem::addFolders(QStandardItem *parent, QList<Folder *> folders, const QString &email)
{
    for (auto *folder : folders) {
        FolderItem *item = new FolderItem(m_account, folder);
        parent->appendRow(item);
        if (folder->hasChildren()) {
            addFolders(item, folder->Children(), email);
        }
    }
}

void AccountItem::getFolders()
{
    ImapService *service = m_account->IMAPService();
    QList<Folder *> *folders = service->getFolders("*");

    addFolders(this, *folders, m_account->Email());
    emit foldersLoadFinished();
}

Account *AccountItem::account() const
{
    return m_account;
}
