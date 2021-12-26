#include "folderlistmodel.h"

#include "session.h"

FolderListModel::FolderListModel(QObject *parent)
    : QStandardItemModel(parent)
{
    setLoading(false);
    auto session = Session::getInstance();

    connect(session, &Session::accountAdded, this, &FolderListModel::addAccount);
}

void FolderListModel::selectFolder(QModelIndex index)
{
    BaseFolderItem *item = static_cast<BaseFolderItem *>(itemFromIndex(index));

    if (item->Type() == "FolderItem") {
        FolderItem *item_ = static_cast<FolderItem *>(itemFromIndex(index));

        QString email = item_->Email();

        for (auto account : qAsConst(m_Accounts)) {
            if (account->Email() == email) {
                account->selectFolder(item_->FullFolderName());
                break;
            }
        }
    }
}

void FolderListModel::addAccount(Account *account)
{
    setLoading(true);
    m_Accounts.append(account);
    AccountItem *item = new AccountItem(account);
    this->appendRow(item);
    connect(item, &AccountItem::foldersLoadFinished, this, &FolderListModel::onFoldersLoadFinished);
}

void FolderListModel::onFoldersLoadFinished()
{
    qDebug() << "loading true";
    setLoading(false);

    if (m_Accounts.length() == 1) {
        m_Accounts.at(0)->selectFolder("INBOX");
    }
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
