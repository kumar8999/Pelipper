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

    bool insertFolders(const QList<Folder *> &folderList);

    QList<ssize_t> getUidList(const QString &foldername);

    QList<Message *> *getAllMessages(const QString &foldername);

    Message *getMessage(const QString &foldername, ssize_t uid, QString &data);

    bool insertAllMessages(const QString &foldername, QList<Message *> *msgList);

    bool insertMessage(const QString &foldername, Message *msg);

    bool deleteMessage(const QString &foldername, QList<ssize_t> uidList);

    QString getParentFolderName(const QString &fullfoldername, const QChar &delimter);

private:
    QString m_Email;
    QString m_CacheFolderPath;
    QMutex m_Mutex;
    QSettings *m_Settings;
};

#endif // IMAPCACHE_H
