#ifndef IMAPMANAGER_H
#define IMAPMANAGER_H

#include "folder.h"
#include "imap.h"
#include "message.h"

#include <QObject>
#include <QQueue>

enum CMD { FOLDER, HEADERS, MSG };

struct Request {
    CMD cmd;
    QString foldername;
    ssize_t uid;
};

class ImapManager : public QObject
{
    Q_OBJECT
public:
    explicit ImapManager(const QString &email, const QString &password, const QString &server, int port, QObject *parent = nullptr);

    void login();

    void getFolders();

    void getAllHeaders(const QString &foldername);

    void getBody(ssize_t uid);

signals:
    void connectedToServer(bool success);
    void loggedIn(bool success);
    void readFoldersFinished(QList<Folder *> *folderList);
    void readAllHeadersFinished(QList<Message *> *MessageList);
    void readBodyFinished(Message *msg);

private:
    void startProcess();
    void processRequest(Request request);

    void readFolders();

    void parseFolders(QStringList folderList);
    void parseFolder(QStringList foldernameList, int index);
    QString getFolderName(QStringList folderList, int index);
    QString getParentFolderName(QStringList folderList, int index);

    void readAllHeaders(QString foldername);

    void readBody(ssize_t uid);

    void startCacheProcess();

private:
    Imap *m_Imap;
    QMutex m_Mutex;
    QString m_Email;
    QString m_Password;
    QString m_Server;
    int m_Port;
    QList<Folder *> *m_FolderList;
    QMap<QString, Folder *> m_Folders;

    bool m_ThreadIsRunning;

    QQueue<Request> m_Request;
};

#endif // IMAPMANAGER_H
