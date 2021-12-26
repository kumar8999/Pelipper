#ifndef IMAP_H
#define IMAP_H

#include <QMap>
#include <QMutex>
#include <QObject>
#include <libetpan/imapdriver_tools.h>
#include <libetpan/libetpan.h>

class Imap : public QObject
{
    Q_OBJECT
public:
    explicit Imap(const QString &email, const QString &password, const QString &server, int port);
    ~Imap();

    bool connect();

    bool login();
    bool logout();

    bool selectFolder(QString folderName);
    bool getFolders(QString folderName, QStringList &folderList);

    bool getUids(const QString &foldername, QList<ssize_t> &uidList);
    bool getBody(ssize_t uid, QString &data);

    bool getAllHeaders(const QString &foldername, QList<ssize_t> headerUidList, QMap<ssize_t, QString> &headerDatas);
    bool getAllBody(QMap<ssize_t, QString> &bodyDatas, QList<ssize_t> nonCachedUidList);

    bool checkInternet();

    bool deleteMessage(const QString &foldername, QList<ssize_t> uidList);
    bool moveMessage(const QString &souceFolderName, const QString &destFolderName, QList<ssize_t> uidList);

private:
    QString m_Email;
    QString m_Password;
    QString m_Server;
    int m_Port;

    QString m_SelectedFolder;

    mailimap *m_Imap;
    QMutex m_Mutex;
};

#endif // IMAP_H
