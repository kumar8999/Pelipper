#ifndef IDLEMANAGER_H
#define IDLEMANAGER_H

#include "imapservice.h"

#include <QObject>
#include <QSocketDescriptor>

class IdleManager : public QObject
{
    Q_OBJECT
public:
    explicit IdleManager(ImapService *imapService, QObject *parent = nullptr);

    void start();

signals:

private slots:

    void onActivated(QSocketDescriptor socket, QSocketNotifier::Type type);

private:
private:
    ImapService *m_ImapService;
};

#endif // IDLEMANAGER_H
