#ifndef MESSAGELISTTHREAD_H
#define MESSAGELISTTHREAD_H

#include "../backend/account.h"

#include <QObject>

class MessageListThread : public QObject
{
    Q_OBJECT
public:
    explicit MessageListThread(QObject *parent = nullptr);

    void setFolder(QHash<Account *, Folder *> *folderAccounts);

    void setStopThread(bool newStopThread);

public slots:
    void loadFolders();

signals:

    void messageReadReady(QList<Message *> *m_MessageList);

private:
    QHash<Account *, Folder *> *m_FolderAccounts;
    bool m_stopThread;
};

#endif // MESSAGELISTTHREAD_H
