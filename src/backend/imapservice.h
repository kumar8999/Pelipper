#ifndef IMAP_H
#define IMAP_H

#include "folder.h"
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

    bool getUids(const QString &foldername, QList<ssize_t> &uidList);
    Message *getBody(ssize_t uid);

    QList<Message *> *getAllHeaders(const QString &foldername, QList<ssize_t> headerUidList);
    bool getAllBody(QMap<ssize_t, QString> &bodyDatas, QList<ssize_t> nonCachedUidList);
    bool getFlags(const QString &foldername, QList<ssize_t> uidList, QMap<ssize_t, QList<bool>> &flagsList);

    bool checkInternet();

    bool deleteMessage(const QString &foldername, QList<ssize_t> uidList);
    bool moveMessage(const QString &souceFolderName, const QString &destFolderName, QList<ssize_t> uidList);

private:
    void parseFolders(QMap<QString, mailimap_mailbox_list *> folderMap, QList<Folder *> *&folderList);
    void parseFolder(QStringList foldernameList, int index, mailimap_mailbox_list *mb, QList<Folder *> *&folderList);
    QString getFolderName(QStringList folderList, int index);
    QString getParentFolderName(QStringList folderList, int index);

private:
    QString m_Email;
    QString m_Password;
    QString m_Server;
    int m_Port;

    QString m_SelectedFolder;

    mailimap *m_Imap;
    QMutex m_Mutex;

    QMap<QString, Folder *> m_Folders;
};

#endif // IMAP_H
