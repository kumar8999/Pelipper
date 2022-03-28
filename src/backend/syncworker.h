#ifndef SYNCWORKER_H
#define SYNCWORKER_H

#include "account.h"

#include <QObject>

class SyncWorker : public QObject
{
    Q_OBJECT
public:
    explicit SyncWorker(QObject *parent = nullptr);

    Account *account() const;

    void setAccount(Account *newAccount);

    void fetchFolders();

    void fetchHeaders();

    void fetchMessage();

    void setSelectedFolder(const QString &newSelectedFolder);

signals:

    void foldersReadFinished(QList<Folder *> *folders);

    void messagesReadFinished(QList<Message *> *messages);

    void messageReadFinished(Message *msg);

private:
    void loadcacheFolders();

    void loadcacheMessages(const QString &folder);

    void loadFolders();

    void loadMessages(const QString &folder, const FetchType &fetchType);

    void loadMessage(const QString &folder, const ssize_t &uid);

private:
    Account *m_account;

    QString m_selectedFolder;

    ssize_t m_uid;
};

#endif // SYNCWORKER_H
