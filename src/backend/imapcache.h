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

    QStringList fetchFoldersStr();

    bool insertFoldersStr(const QStringList &folderList);

    bool insertFolders(const QList<Folder *> &folderList);

    void insertUidList(const QString &foldername, const QList<ssize_t> &uidList);

    QList<ssize_t> getUidList(const QString &foldername);

    QList<Message *> *getAllMessages(const QString &foldername);

    Message *getMessage(const QString &foldername, ssize_t uid, QString &data);

    Flags *getFlags(const ssize_t &uid);

    bool insertAllMessages(const QString &foldername, QList<Message *> *msgList);

    bool insertMessage(const QString &foldername, Message *msg);

    bool saveFlags(const ssize_t &uid, Flags *flags);

    bool deleteMessage(const QString &foldername, QList<ssize_t> uidList);

    QString getParentFolderName(const QString &fullfoldername, const QChar &delimter);

private:
    QJsonObject insertFolderObj(Folder *folder);

    Folder *getFolderObj(QJsonObject obj);

private:
    QString m_Email;
    QString m_cacheFilePath;
};

#endif // IMAPCACHE_H
