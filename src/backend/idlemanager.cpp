#include "idlemanager.h"

#include <QDebug>
#include <QSocketNotifier>
#include <QtConcurrent>

IdleManager::IdleManager(ImapService *imapService, QObject *parent)
    : QObject{parent}
{
    m_ImapService = imapService;
    m_ImapService->connect();
    m_ImapService->login();
}

void IdleManager::start()
{
    int fd = m_ImapService->idleStart("INBOX");

    QSocketNotifier *socketNotifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);

    connect(socketNotifier, &QSocketNotifier::activated, this, &IdleManager::onActivated);

    socketNotifier->setEnabled(true);
}

void IdleManager::onActivated(QSocketDescriptor socket, QSocketNotifier::Type type)
{
    qDebug() << "descriptor";
}
