#include "folderlistmodel.h"

#include "../backend/session.h"

#include <QDebug>
#include <QtConcurrent>

FolderListModel::FolderListModel(QObject *parent)
    : m_loading(false)
    , QStandardItemModel(parent)
{
    auto session = Session::getInstance();
    m_settings = new Settings(this);

    connect(session, &Session::accountAdded, this, &FolderListModel::addAccount);
}

void FolderListModel::selectFolder(QModelIndex index)
{
    BaseFolderItem *item = static_cast<BaseFolderItem *>(itemFromIndex(index));

    if (item->Type() == "FolderItem") {
        FolderItem *item_ = static_cast<FolderItem *>(itemFromIndex(index));

        Account *account = item_->account();

        QHash<Account *, Folder *> *accountFolder = new QHash<Account *, Folder *>();

        accountFolder->insert(account, item_->folder());

        m_settings->setSelectedFolder(account->Email(), item_->folder()->FullName());

        emit folderSelected(accountFolder);
    }
}

void FolderListModel::addAccount(Account *account)
{
    setLoading(true);

    AccountItem *item = new AccountItem(account);
    this->appendRow(item);
    connect(item, &AccountItem::foldersLoadFinished, this, &FolderListModel::onFoldersLoadFinished);
}

void FolderListModel::onFoldersLoadFinished()
{
    setLoading(false);

    AccountItem *item = static_cast<AccountItem *>(sender());
    Account *account = item->account();
    ImapService *service = account->IMAPService();

    QString email;
    QString foldername;
    m_settings->getSelectedFolder(email, foldername);

    m_selectFolderThread.cancel();
    QHash<Account *, Folder *> *accountFolder = new QHash<Account *, Folder *>();

    //    if (email == account->Email()) {
    //        m_selectFolderThread = QtConcurrent::run([&]() {
    //            Folder *folder = service->getFolder(foldername);

    //            if (folder != nullptr) {
    //                accountFolder->insert(account, folder);
    //            }
    //        });
    //    } else {
    //        m_selectFolderThread = QtConcurrent::run([&]() {
    //            Folder *folder = service->getFolder("INBOX");

    //            if (folder != nullptr) {
    //                accountFolder->insert(account, folder);
    //            }
    //        });
    //    }
    //    emit folderSelected(accountFolder);
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
