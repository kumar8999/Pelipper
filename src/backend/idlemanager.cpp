#include "idlemanager.h"

#include "imapservice.h"

#include <QDebug>
#include <QSocketNotifier>
#include <QtConcurrent>

IdleManager::IdleManager(ImapService *imapService, QObject *parent)
    : m_folderName("INBOX")
    , QObject{parent}
{
    m_ImapService = imapService;
    m_ImapService->connect();
    m_ImapService->login();
}

void IdleManager::start()
{
    m_uidList = m_ImapService->getUids(m_folderName);

    int fd = m_ImapService->idleStart("INBOX");

    qDebug() << "file descriptor" << fd;

    QSocketNotifier *socketNotifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);

    connect(socketNotifier, &QSocketNotifier::activated, this, &IdleManager::onActivated);

    socketNotifier->setEnabled(true);
}

void IdleManager::onActivated(QSocketDescriptor socket, QSocketNotifier::Type type)
{
    qDebug() << "descriptor";

    QList<ssize_t> uidList = m_ImapService->getUids(m_folderName);

    if (uidList.length() > m_uidList.length()) {
        emit newMessagesRecieved(m_folderName);
    }
}

const QString &IdleManager::folderName() const
{
    return m_folderName;
}

void IdleManager::setFolderName(const QString &newFolderName)
{
    m_folderName = newFolderName;
}
