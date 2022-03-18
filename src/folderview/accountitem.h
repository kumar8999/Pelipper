#ifndef ACCOUNTITEM_H
#define ACCOUNTITEM_H

#include "../backend/account.h"
#include "../backend/folder.h"
#include "folderitem.h"

#include <QObject>
#include <QStandardItem>

class AccountItem : public QObject, public QStandardItem
{
    Q_OBJECT
public:
    explicit AccountItem(Account *account);

    void loadFolders();

    Account *account() const;

    const QStringList &folderList() const;

    const QString &email() const;

signals:
    void foldersLoadFinished();

private:
    void addFolders(QStandardItem *parent, QList<Folder *> folders, const QString &email);

    void getFolders();

private:
    QString m_email;
    Account *m_account;
    QStringList m_folderList;
};

#endif // ACCOUNTITEM_H
