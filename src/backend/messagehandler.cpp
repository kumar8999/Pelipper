#include "messagehandler.h"

#include <QDebug>

MessageHandler::MessageHandler(QObject *parent)
    : QObject{parent}
{
    m_syncmanager = nullptr;
    m_messages = new QList<Message *>();
}

SyncManager *MessageHandler::syncmanager() const
{
    return m_syncmanager;
}

void MessageHandler::setSyncmanager(SyncManager *newSyncmanager)
{
    m_syncmanager = newSyncmanager;

    connect(m_syncmanager, &SyncManager::messagesReadFinished, this, &MessageHandler::onMessageReadFinished);
}

void MessageHandler::fetchHeaders(QHash<Account *, QString> *accountFolder)
{
    qDebug() << "fetching headers";
    m_messages->clear();
    m_syncmanager->fetchHeaders(accountFolder);
}

void MessageHandler::onMessageReadFinished(Account *account, const QString &folder, QList<Message *> *msgList)
{
    m_messages->append(*msgList);
    emit messageLoadFinished();
}

QList<Message *> *MessageHandler::messages() const
{
    return m_messages;
}
