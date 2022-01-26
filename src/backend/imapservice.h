#ifndef IMAP_H
#define IMAP_H

#include "folder.h"
#include "imapcache.h"
#include "message.h"

#include <QMap>
#include <QMutex>
#include <QObject>
#include <libetpan/imapdriver_tools.h>
#include <libetpan/libetpan.h>

class ImapService : public QObject
{
    Q_OBJECT
public:
    explicit ImapService(const QString &email, const QString &password, const QString &server, int port);
    ~ImapService();

    bool connect();

    bool login();
    bool logout();

    bool selectFolder(QString folderName);
    QList<Folder *> *getFolders(QString folderName);

    QList<ssize_t> getNonCachedUids(const QString &foldername);
    Message *getBody(ssize_t uid);
    QList<Message *> *getAllMessage(const QString &foldername);

    QList<Message *> *getAllHeaders(const QString &foldername);

    bool checkInternet();

    bool deleteMessage(const QString &foldername, QList<ssize_t> uidList);
    bool moveMessage(const QString &souceFolderName, const QString &destFolderName, QList<ssize_t> uidList);

private:
    QString getParentFolderName(QString fullfoldername, QChar delimter);

private:
    QString m_Email;
    QString m_Password;
    QString m_Server;
    int m_Port;

    bool m_Connected;
    bool m_LoggedIn;

    QString m_SelectedFolder;

    mailimap *m_Imap;
    QMutex m_Mutex;

    ImapCache *m_ImapCache;

    QMap<QString, Folder *> m_Folders;
};

#endif // IMAP_H
