#ifndef FOLDERLISTMODEL_H
#define FOLDERLISTMODEL_H

#include "../backend/account.h"
#include "../backend/folderhandler.h"
#include "../backend/settings.h"
#include "accountitem.h"

#include <QFuture>
#include <QStandardItemModel>

class FolderListModel : public QStandardItemModel
{
    Q_OBJECT

    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)

public:
    FolderListModel(QObject *parent = nullptr);

    bool loading() const;

    void setLoading(bool newLoading);

    void setFolderList(const QStringList &newFolderList);

    const QStringList &folderList() const;

    void setFolderhandler(FolderHandler *newFolderhandler);

public slots:
    void selectFolder(QModelIndex index);

signals:
    void loadingChanged();

    void folderSelected(QHash<Account *, Folder *> *accountFolders);

private slots:
    void onFoldersLoadFinished();

private:
    void addFolders(QStandardItem *parent, QList<Folder *> folders);

private:
    bool m_loading;
    Settings *m_settings;
    FolderHandler *m_folderhandler;
    QStringList m_accountList;
};

#endif // FOLDERLISTMODEL_H
