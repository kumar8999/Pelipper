#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "folder.h"
#include "imap.h"
#include "imapcache.h"

#include <QObject>

class Account : public QObject
{
    Q_OBJECT
public:
    explicit Account(const QString &username,
                     const QString &email,
                     const QString &password,
                     const QString &imapServer,
                     const int &imapPort,
                     const QString &smtpServer,
                     const int &smtpPort,
                     QObject *parent = nullptr);

    ~Account();

    bool getFolders();

    bool selectFolder(QString foldername);

    bool getHeaders(QString foldername, QMap<ssize_t, QString> &headerList);

    bool getBody(ssize_t uid, QString &data);

    const QString &Email() const;

    bool deleteMessage(QList<ssize_t> uidList);

    bool moveMessage(const QString &sourceFoldername, const QString &destFolderName, QList<ssize_t> uidList);

signals:
    void folderSelected(QString foldername);
    void folderReadFinished(QList<Folder *> *folders);

private:
    void startCacheThread();

    void parseFolders(QStringList folderList);
    void parseFolder(QStringList foldernameList, int index);
    QString getFolderName(QStringList folderList, int index);
    QString getParentFolderName(QStringList folderList, int index);

private:
    QString m_Username;
    QString m_Email;
    QString m_Password;
    QString m_ImapServer;
    QString m_SmtpServer;
    int m_ImapPort;
    int m_SmtpPort;

    QString m_SelectFolder;

    Imap *m_Imap;
    ImapCache *m_ImapCache;

    QList<Folder *> *m_FolderList;
    QMap<QString, Folder *> m_Folders;
};

#endif // ACCOUNT_H
