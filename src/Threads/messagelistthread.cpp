#include "messagelistthread.h"

MessageListThread::MessageListThread(QObject *parent)
    : m_stopThread(false)
    , QObject{parent}
{
}

void MessageListThread::setFolder(QHash<Account *, Folder *> *folderAccounts)
{
    m_stopThread = false;
    m_FolderAccounts = folderAccounts;
}

void MessageListThread::loadFolders()
{
    QHashIterator<Account *, Folder *> iter(*m_FolderAccounts);

    while (iter.hasNext()) {
        iter.next();
        Account *account = iter.key();
        Folder *folder = iter.value();

        if (m_stopThread)
            break;

        ImapService *service = account->IMAPService();
        QList<Message *> *msgList = service->getAllHeaders(folder->FullName());

        if (m_stopThread)
            break;

        emit messageReadReady(msgList);
    }
}

void MessageListThread::setStopThread(bool newStopThread)
{
    m_stopThread = newStopThread;
}
