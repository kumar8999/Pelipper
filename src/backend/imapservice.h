#ifndef IMAPSERVICE_H
#define IMAPSERVICE_H

#include "folder.h"
#include "message.h"

#include <QMap>
#include <QMutex>
#include <QObject>

#include <libetpan/libetpan.h>

enum ConnectionType { ConnectionTypeNone, ConnectionTypeStartTLS, ConnectionSSL };

enum FetchType {
    None = 1 << 0,
    Header = 1 << 1,
    Body = 1 << 2,
    BodyWithNoSeen = 1 << 3,
    Flag = 1 << 4,
    All = Header | Body | Flag,
    AllWithNoSeen = Header | Body | Flag
};

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

    QList<Folder *> *getFolders();

    QStringList fetchFoldersStringList();

    QList<ssize_t> getUids(const QString &foldername);

    QList<Message *> *getAllHeaders(const QString &foldername, const QList<ssize_t> &uidList);

    Message *getBody(ssize_t uid);

    QList<Message *> *getAllMessage(const QString &foldername);

    QList<Message *> *fetchMessage(const QString &foldername, const QList<ssize_t> &uidList, FetchType fetchType);

    bool checkInternet();

    bool deleteMessage(const QString &foldername, QList<ssize_t> uidList);

    bool moveMessage(const QString &souceFolderName, const QString &destFolderName, QList<ssize_t> uidList);

    int idleStart(const QString &foldername);

    bool idleDone();

private:
    QString getParentFolderName(const QString &fullfoldername, const QChar &delimter);

    void parseFlags(mailimap_flag_fetch *flagFetch, Flags *flags);

private:
    QString m_email;
    QString m_password;
    QString m_server;
    int m_port;
    ConnectionType m_connectionType;

    mailimap *m_imap;
    QMutex m_mutex;
};

#endif // IMAPSERVICE_H
