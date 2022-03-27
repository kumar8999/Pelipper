#ifndef IDLEMANAGER_H
#define IDLEMANAGER_H

#include <QObject>
#include <QSocketDescriptor>

class ImapService;

class IdleManager : public QObject
{
    Q_OBJECT
public:
    explicit IdleManager(ImapService *imapService, QObject *parent = nullptr);

    void start();

    const QString &folderName() const;
    void setFolderName(const QString &newFolderName);

signals:

    void newMessagesRecieved(QString foldername);

private slots:

    void onActivated(QSocketDescriptor socket, QSocketNotifier::Type type);

private:
private:
    ImapService *m_ImapService;

    QString m_folderName;

    QList<ssize_t> m_uidList;
};

#endif // IDLEMANAGER_H
