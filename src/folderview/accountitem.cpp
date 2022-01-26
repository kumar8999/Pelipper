#include "accountitem.h"

#include <QtConcurrent>

AccountItem::AccountItem(Account *account)
{
    m_Account = account;
    setText(account->Email());
    qDebug() << "account item";
    loadFolders();
}

void AccountItem::loadFolders()
{
    QtConcurrent::run(this, &AccountItem::getFolders);
}

void AccountItem::addFolders(QStandardItem *parent, QList<Folder *> folders, QString email)
{
    for (auto *folder : folders) {
        FolderItem *item = new FolderItem(m_Account, folder);
        parent->appendRow(item);
        if (folder->hasChildren()) {
            addFolders(item, folder->Children(), email);
        }
    }
}

void AccountItem::getFolders()
{
    ImapService *service = m_Account->IMAPService();
    QList<Folder *> *folders = service->getFolders("*");

    qDebug() << folders;

    addFolders(this, *folders, m_Account->Email());
    emit foldersLoadFinished();
}

Account *AccountItem::account() const
{
    return m_Account;
}
