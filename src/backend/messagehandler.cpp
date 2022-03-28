#include "messagehandler.h"

#include <QDebug>

MessageHandler::MessageHandler(QObject *parent)
    : QObject{parent}
{
    m_syncmanager = nullptr;
}

void MessageHandler::fetchHeaders(QHash<Account *, Folder *> *accountFolder)
{
    m_syncmanager->fetchHeaders(accountFolder);
}

void MessageHandler::fetchMessage(Account *account, const QString &foldername, const ssize_t &uid)
{
    m_syncmanager->fetchMessages(account, foldername, uid);
}

void MessageHandler::setSyncmanager(SyncManager *newSyncmanager)
{
    m_syncmanager = newSyncmanager;

    connect(m_syncmanager, &SyncManager::messagesReadFinished, this, &MessageHandler::onMessagesReadFinished);
}

void MessageHandler::clearAllMessages()
{
    m_messages.clear();
}

void MessageHandler::onMessagesReadFinished(QMap<QString, QList<Message *> *> messages)
{
    QMapIterator<QString, QList<Message *> *> messageIter(messages);

    while (messageIter.hasNext()) {
        messageIter.next();

        m_messages[messageIter.key()] = messageIter.value();
    }

    emit messageLoadFinished();
}

const QMap<QString, QList<Message *> *> &MessageHandler::messages() const
{
    return m_messages;
}
