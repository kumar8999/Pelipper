#ifndef SYNCWORKER_H
#define SYNCWORKER_H

#include "account.h"

#include <QObject>

class SyncWorker : public QObject
{
    Q_OBJECT
public:
    explicit SyncWorker(QObject *parent = nullptr);

    void loadCacheFolders();

    void fetchFolders();

    void fetchCacheFolders();

    void setAccount(Account *newAccount);

    Account *account() const;

    void fetchHeaders();

    void setSelectedFolder(const QString &newSelectedFolder);

    void syncFolders();

    void syncFolder(QString folder);

    const QString &getSelectedFolder() const;

signals:
    void foldersReadFinished(QList<Folder *> *folders);

    void messagesReadFinished(QList<Message *> *msgList);

    void cacheMessageReadFinished(QList<ssize_t> uidList);

    void cachedUidList(QList<ssize_t> uidList);

    void fetchedUidList(QString folder, QList<ssize_t> uidList);

private:
private:
    Account *m_account;

    ImapService *m_imapService;

    QString selectedFolder;
};

#endif // SYNCWORKER_H
