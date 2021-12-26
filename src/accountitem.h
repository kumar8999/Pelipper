#ifndef ACCOUNTITEM_H
#define ACCOUNTITEM_H

#include "account.h"
#include "folder.h"
#include "folderitem.h"

#include <QObject>
#include <QStandardItem>

class AccountItem : public QObject, public QStandardItem
{
    Q_OBJECT
public:
    AccountItem(Account *account);

    void loadFolders();

signals:
    void foldersLoadFinished();

private slots:
    void onFoldersReadFinished(QList<Folder *> *folderList);

private:
    void addFolders(QStandardItem *parent, QList<Folder *> folders, QString email);

    void parseFolders(QStringList folderList);
    void parseFolder(QStringList foldernameList, int index);
    QString getFolderName(QStringList folderList, int index);
    QString getParentFolderName(QStringList folderList, int index);

private:
    QList<Folder *> m_FolderList;
    QMap<QString, Folder *> m_Folders;

    QString m_Email;
    Account *m_Account;
};

#endif // ACCOUNTITEM_H
