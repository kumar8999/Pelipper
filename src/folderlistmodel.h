#ifndef FOLDERLISTMODEL_H
#define FOLDERLISTMODEL_H

#include "account.h"
#include "accountitem.h"

#include <QStandardItemModel>

class FolderListModel : public QStandardItemModel
{
    Q_OBJECT

    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)
public:
    explicit FolderListModel(QObject *parent = nullptr);

    bool loading() const;
    void setLoading(bool newLoading);

public slots:
    void selectFolder(QModelIndex index);

signals:
    void loadingChanged();

private slots:
    void addAccount(Account *account);

    void onFoldersLoadFinished();

private:
    void getFolders();

private:
    QList<Account *> m_Accounts;
    bool m_loading;
};

#endif // FOLDERLISTMODEL_H
