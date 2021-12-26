#ifndef IMAPCACHE_H
#define IMAPCACHE_H

#include <QMutex>
#include <QObject>
#include <QSettings>

class ImapCache : public QObject
{
    Q_OBJECT
public:
    explicit ImapCache(const QString &email, QObject *parent = nullptr);

    bool getFolders(QStringList &folderList);

    bool insertFolders(QStringList folderList);

    bool getUidList(QString foldername, QList<ssize_t> &uidList);

    bool getAllMessages(QString foldername, QMap<ssize_t, QString> &headerData);

    bool getBody(QString foldername, ssize_t uid, QString &data);

    bool insertMessages(const QString &foldername, QMap<ssize_t, QString> bodyData);

    bool deleteMessage(const QString &foldername, QList<ssize_t> uidList);

private:
    QString m_Email;
    QMutex m_Mutex;
    QSettings *m_Settings;
};

#endif // IMAPCACHE_H
