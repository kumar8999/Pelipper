#ifndef FOLDERHANDLER_H
#define FOLDERHANDLER_H

#include "syncmanager.h"

#include <QObject>

class FolderHandler : public QObject
{
    Q_OBJECT
public:
    explicit FolderHandler(QObject *parent = nullptr);

    void setSyncmanager(SyncManager *newSyncmanager);

    const QMap<Account *, QList<Folder *> *> &folders() const;

signals:
    void folderReadFinished();

private slots:
    void onFolderReadFinished(Account *account, QList<Folder *> *folders);

private:
    SyncManager *m_syncmanager;

    QMap<Account *, QList<Folder *> *> m_folders;
};

#endif // FOLDERHANDLER_H
