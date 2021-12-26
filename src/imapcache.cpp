#include "imapcache.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>

ImapCache::ImapCache(const QString &email, QObject *parent)
    : m_Email(email)
    , QObject(parent)
{
    QString accountHash = QString(QCryptographicHash::hash((m_Email.toUtf8()), QCryptographicHash::Md5).toHex());
    QString cachePath = QDir::homePath() + "/.config/Pelipper/";

    QDir dir;
    dir.mkpath(cachePath + accountHash);

    m_Settings = new QSettings(cachePath + accountHash + "_", QSettings::IniFormat, this);
}

bool ImapCache::getFolders(QStringList &folderList)
{
    folderList = m_Settings->value("folders", QStringList()).toStringList();

    return true;
}

bool ImapCache::insertFolders(QStringList folderList)
{
    m_Settings->setValue("folders", folderList);

    return true;
}

bool ImapCache::getUidList(QString foldername, QList<ssize_t> &uidList)
{
    m_Settings->beginGroup("uids");
    QStringList uids = m_Settings->value(foldername, QStringList()).toStringList();
    m_Settings->endGroup();
    for (const auto &uid : qAsConst(uids)) {
        uidList.append(uid.toLong());
    }

    return true;
}

bool ImapCache::getAllMessages(QString foldername, QMap<ssize_t, QString> &headerData)
{
    QString accountHash = QString(QCryptographicHash::hash((m_Email.toUtf8()), QCryptographicHash::Md5).toHex());
    QString cachePath = QDir::homePath() + "/.config/Pelipper/" + accountHash;
    QDir dir(cachePath + "/MAILS");

    if (!dir.exists()) {
        return false;
    }

    m_Settings->beginGroup("uids");
    QStringList uids = m_Settings->value(foldername, QStringList()).toStringList();
    m_Settings->endGroup();
    QFileInfoList filenameList = dir.entryInfoList(uids);

    for (const auto &fileinfo : qAsConst(filenameList)) {
        QFile file(fileinfo.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;

        QTextStream in(&file);
        QString data;
        while (!in.atEnd()) {
            QString line = in.readAll();
            data.append(line);
        }

        headerData.insert(fileinfo.fileName().toULong(), data);
    }

    return true;
}

bool ImapCache::getBody(QString foldername, ssize_t uid, QString &data)
{
    QString accountHash = QString(QCryptographicHash::hash((m_Email.toUtf8()), QCryptographicHash::Md5).toHex());
    QString cachePath = QDir::homePath() + "/.config/Pelipper/" + accountHash;
    QDir dir(cachePath + "/MAILS");

    if (!dir.exists()) {
        return false;
    }

    QFileInfoList fileinfoList = dir.entryInfoList(QStringList() << QString::number(uid));

    if (fileinfoList.isEmpty()) {
        return false;
    }

    QFileInfo fileinfo = fileinfoList.at(0);

    QFile file(fileinfo.absoluteFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readAll();
        data.append(line);
    }

    return true;
}

bool ImapCache::insertMessages(const QString &foldername, QMap<ssize_t, QString> bodyData)
{
    QString accountHash = QString(QCryptographicHash::hash((m_Email.toUtf8()), QCryptographicHash::Md5).toHex());

    QString cachePath = QDir::homePath() + "/.config/Pelipper/" + accountHash;

    QDir dir(cachePath + "/MAILS");

    if (!dir.exists()) {
        bool result = dir.mkpath(cachePath + "/MAILS");
    }

    QStringList uidList;

    QMapIterator<ssize_t, QString> bodyIter(bodyData);

    while (bodyIter.hasNext()) {
        bodyIter.next();

        uidList.append(QString::number(bodyIter.key()));

        QFile file(dir.path() + "/" + QString::number(bodyIter.key()));
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return false;

        QTextStream out(&file);
        out << bodyIter.value();
    }

    m_Settings->beginGroup("uids");
    m_Settings->setValue(foldername, uidList);
    m_Settings->endGroup();
    return true;
}

bool ImapCache::deleteMessage(const QString &foldername, QList<ssize_t> uidList)
{
    QString accountHash = QString(QCryptographicHash::hash((m_Email.toUtf8()), QCryptographicHash::Md5).toHex());
    QString cachePath = QDir::homePath() + "/.config/Pelipper/" + accountHash;
    QDir dir(cachePath + "/MAILS");

    if (!dir.exists()) {
        return false;
    }

    QStringList filterList;
    for (auto uid : uidList) {
        filterList.append(QString::number(uid));
    }

    QFileInfoList fileinfoList = dir.entryInfoList(filterList);

    if (fileinfoList.isEmpty()) {
        return false;
    }

    for (auto fileinfo : fileinfoList) {
        QFile file(fileinfo.absoluteFilePath());
        if (!file.remove()) {
            return false;
        }
    }

    return true;
}
