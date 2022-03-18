#ifndef FOLDERLISTMODEL_H
#define FOLDERLISTMODEL_H

#include "../backend/account.h"
#include "../backend/settings.h"
#include "accountitem.h"

#include <QFuture>
#include <QStandardItemModel>

class FolderListModel : public QStandardItemModel
{
    Q_OBJECT

    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)
    Q_PROPERTY(QStringList folderList READ folderList WRITE setFolderList NOTIFY folderListChanged)

public:
    FolderListModel(QObject *parent = nullptr);

    bool loading() const;

    void setLoading(bool newLoading);

    void setFolderList(const QStringList &newFolderList);

    const QStringList &folderList() const;

public slots:
    void selectFolder(QModelIndex index);

    void loadFolderList(const QString &accountEmail = "");
signals:
    void loadingChanged();

    void folderSelected(QHash<Account *, Folder *> *accountFolders);

    void folderListChanged();

private slots:
    void addAccount(Account *account);

    void onFoldersLoadFinished();

private:
    bool m_loading;
    Settings *m_settings;
    QFuture<void> m_selectFolderThread;

    QStringList m_folderList;
};

#endif // FOLDERLISTMODEL_H
