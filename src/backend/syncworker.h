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

signals:

    void foldersReadFinished(QList<Folder *> *folders);

    void messagesReadFinished(QList<Message *> *messages);

private:
    void loadcacheFolders();

    void loadcacheMessages(const QString &folder);

    void loadFolders();

    void loadMessages(const QString &folder);

private:
    Account *m_account;

    QString m_selectedFolder;
};

#endif // SYNCWORKER_H
