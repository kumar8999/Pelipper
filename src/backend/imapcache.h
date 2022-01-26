#ifndef IMAPCACHE_H
#define IMAPCACHE_H

#include "folder.h"
#include "message.h"

#include <QMutex>
#include <QObject>
#include <QSettings>

class ImapCache : public QObject
{
    Q_OBJECT
public:
    explicit ImapCache(const QString &email, QObject *parent = nullptr);

    QList<Folder *> *getFolders();

    bool insertFolders(QList<Folder *> folderList);

    QList<ssize_t> getUidList(QString foldername);

    QList<Message *> *getAllMessages(QString foldername);

    Message *getMessage(QString foldername, ssize_t uid, QString &data);

    bool insertAllMessages(const QString &foldername, QList<Message *> *msgList);

    bool insertMessage(const QString &foldername, Message *msg);

    bool deleteMessage(const QString &foldername, QList<ssize_t> uidList);

    QString getParentFolderName(QString fullfoldername, QChar delimter);

private:
    QString m_Email;
    QMutex m_Mutex;
    QSettings *m_Settings;
};

#endif // IMAPCACHE_H
