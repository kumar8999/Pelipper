#include "folderlistmodel.h"

#include <QDebug>

FolderListModel::FolderListModel(QObject *parent)
    : m_loading(false)
    , QStandardItemModel(parent)
{
    m_folderhandler = nullptr;
    m_settings = new Settings(this);
}

void FolderListModel::selectFolder(QModelIndex index)
{
    BaseFolderItem *item = static_cast<BaseFolderItem *>(itemFromIndex(index));

    if (item->Type() == "FolderItem") {
        FolderItem *item_ = static_cast<FolderItem *>(itemFromIndex(index));

        Account *account = item_->account();

        QHash<Account *, Folder *> *accountFolder = new QHash<Account *, Folder *>();

        qDebug() << account->Email() << "account;";

        accountFolder->insert(account, item_->folder());

        m_settings->setSelectedFolder(account->Email(), item_->folder()->FullName());

        emit folderSelected(accountFolder);
    }
}

void FolderListModel::onFoldersLoadFinished()
{
    setLoading(false);

    QMap<Account *, QList<Folder *> *> folders = m_folderhandler->folders();
    QMapIterator<Account *, QList<Folder *> *> folderIter(folders);

    while (folderIter.hasNext()) {
        folderIter.next();

        Account *account = folderIter.key();
        QList<Folder *> *folderList = folderIter.value();

        AccountItem *item = nullptr;
        int row = -1;

        if (m_accountList.contains(account->Email())) {
            for (int i = 0; i < this->rowCount(); i++) {
                AccountItem *accountItem = static_cast<AccountItem *>(this->item(i));

                if (accountItem->email() == account->Email()) {
                    row = i;
                    item = accountItem;
                    item->clearAll();
                }
            }
        }

        if (item == nullptr) {
            item = new AccountItem(account);
        }

        addFolders(item, *folderList, account);

        if (row == -1) {
            this->appendRow(item);
            m_accountList.append(account->Email());
        }
    }
}

void FolderListModel::addFolders(QStandardItem *parent, QList<Folder *> folders, Account *account)
{
    for (auto *folder : folders) {
        FolderItem *item = new FolderItem(account, folder);
        parent->appendRow(item);
        if (folder->hasChildren()) {
            addFolders(item, folder->Children(), account);
        }
    }
}

void FolderListModel::setFolderhandler(FolderHandler *newFolderhandler)
{
    m_folderhandler = newFolderhandler;

    connect(m_folderhandler, &FolderHandler::folderReadFinished, this, &FolderListModel::onFoldersLoadFinished);
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
