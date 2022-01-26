#include "folderlistmodel.h"

#include "../backend/session.h"

#include <QDebug>

FolderListModel::FolderListModel(QObject *parent)
    : m_loading(false)
    , QStandardItemModel(parent)
{
    auto session = Session::getInstance();

    connect(session, &Session::accountAdded, this, &FolderListModel::addAccount);
}

void FolderListModel::selectFolder(QModelIndex index)
{
    BaseFolderItem *item = static_cast<BaseFolderItem *>(itemFromIndex(index));

    if (item->Type() == "FolderItem") {
        FolderItem *item_ = static_cast<FolderItem *>(itemFromIndex(index));

        Account *account = item_->account();

        QHash<Account *, Folder *> accountFolder;

        accountFolder[account] = item_->folder();

        emit folderSelected(accountFolder);
    }
}

void FolderListModel::addAccount(Account *account)
{
    setLoading(true);

    qDebug() << "folder add account";

    AccountItem *item = new AccountItem(account);
    this->appendRow(item);
    connect(item, &AccountItem::foldersLoadFinished, this, &FolderListModel::onFoldersLoadFinished);
}

void FolderListModel::onFoldersLoadFinished()
{
    setLoading(false);
}

bool FolderListModel::loading() const
{
    return m_loading;
}

void FolderListModel::setLoading(bool newLoading)
{
    if (m_loading == newLoading)
        return;
    m_loading = newLoading;
    emit loadingChanged();
}
