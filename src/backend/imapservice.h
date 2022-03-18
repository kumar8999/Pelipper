#ifndef IMAP_H
#define IMAP_H

#include "folder.h"
#include "imapcache.h"
#include "message.h"

#include <QMap>
#include <QMutex>
#include <QObject>

#include <libetpan/libetpan.h>

enum ConnectionType { ConnectionTypeNone, ConnectionTypeStartTLS, ConnectionSSL };

class ImapService : public QObject
{
    Q_OBJECT
public:
    explicit ImapService(const QString &email,
                         const QString &password,
                         const QString &server,
                         int port,
                         ConnectionType connectionType,
                         QObject *parent = nullptr);
    ~ImapService();

    bool connect();

    bool login();

    bool logout();

    bool selectFolder(const QString &folderName);

    QList<Folder *> *getFolders(QStringList &folderListstr);
    Folder *getFolder(const QString &foldername);

    QList<ssize_t> getNonCachedUids(const QString &foldername);

    QList<Message *> *getAllHeaders(const QString &foldername);

    Message *getBody(ssize_t uid);

    QList<Message *> *getAllMessage(const QString &foldername);

    bool checkInternet();

    bool deleteMessage(const QString &foldername, QList<ssize_t> uidList);

    bool moveMessage(const QString &souceFolderName, const QString &destFolderName, QList<ssize_t> uidList);

    int idleStart(const QString &foldername);
    bool idleDone();

    void startCache();

private:
    QString getParentFolderName(const QString &fullfoldername, const QChar &delimter);

    void parseFlags(mailimap_flag_fetch *flagFetch, Flags *flags);

private:
    QString m_email;
    QString m_password;
    QString m_server;
    int m_port;

    ConnectionType m_connectionType;

    bool m_connected;
    bool m_loggedIn;
    QString m_selectedFolder;

    mailimap *m_imap;
    QMutex m_mutex;

    ImapCache *m_imapCache;

    QMap<QString, Folder *> m_folders;
};

#endif // IMAP_H
