#ifndef FOLDERLISTMODEL_H
#define FOLDERLISTMODEL_H

#include "../backend/account.h"
#include "accountitem.h"

#include <QStandardItemModel>

class FolderListModel : public QStandardItemModel
{
    Q_OBJECT

    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)

public:
    FolderListModel(QObject *parent = nullptr);

    bool loading() const;
    void setLoading(bool newLoading);

public slots:
    void selectFolder(QModelIndex index);

signals:
    void loadingChanged();

    void folderSelected(QHash<Account *, Folder *> accountFolders);

private slots:
    void addAccount(Account *account);

    void onFoldersLoadFinished();

private:
    void getFolders();

private:
    bool m_loading;
};

#endif // FOLDERLISTMODEL_H