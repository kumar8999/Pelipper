#include "imapcache.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

ImapCache::ImapCache(const QString &email, QObject *parent)
    : m_Email(email)
    , QObject(parent)
{
    QString accountHash = QString(QCryptographicHash::hash((m_Email.toUtf8()), QCryptographicHash::Md5).toHex());
    m_CacheFolderPath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

    if (m_CacheFolderPath == "") {
        m_CacheFolderPath = QDir::homePath() + "/.cache";
    }

    m_CacheFolderPath = m_CacheFolderPath + "/" + accountHash;

    QDir dir;
    dir.mkpath(m_CacheFolderPath);

    m_Settings = new QSettings(m_CacheFolderPath + "/" + accountHash + "_", QSettings::IniFormat, this);
}

QList<Folder *> *ImapCache::getFolders()
{
    QStringList folders = m_Settings->value("folders", QStringList()).toStringList();

    QMap<QString, Folder *> foldersMap;
    QList<Folder *> *folderList = new QList<Folder *>();

    for (int i = 0; i < folders.length(); i++) {
        auto folderfullname = folders.at(i);

        m_Settings->beginGroup("folder." + folderfullname);
        QString fullname = m_Settings->value("fullname").toString();
        QChar delimiter = m_Settings->value("delimiter").toChar();
        m_Settings->endGroup();

        Folder *folder = new Folder(fullname, delimiter);
        QString folderparentName = getParentFolderName(fullname, delimiter);

        foldersMap.insert(fullname, folder);

        if (folderparentName != "") {
            Folder *parentFolder = foldersMap.value(folderparentName);
            parentFolder->appendChild(folder);
            continue;
        }

        folderList->append(folder);
    }

    return folderList;
}

bool ImapCache::insertFolders(const QList<Folder *> &folderList)
{
    QStringList folders;

    for (int i = 0; i < folderList.length(); i++) {
        auto folder = folderList.at(i);
        folders.append(folder->FullName());

        m_Settings->beginGroup("folder." + folder->FullName());
        m_Settings->setValue("fullname", folder->FullName());
        m_Settings->setValue("delimiter", folder->Delimiter());
        m_Settings->endGroup();

        if (folder->hasChildren()) {
            auto childFolderList = folder->Children();

            insertFolders(childFolderList);
        }
    }

    m_Settings->setValue("folders", folders);
    return true;
}

QList<ssize_t> ImapCache::getUidList(const QString &foldername)
{
    m_Settings->beginGroup("uids");
    QStringList uids = m_Settings->value(foldername, QStringList()).toStringList();
    m_Settings->endGroup();

    QList<ssize_t> uidList;
    for (const auto &uid : qAsConst(uids)) {
        uidList.append(uid.toLong());
    }

    return uidList;
}

QList<Message *> *ImapCache::getAllMessages(const QString &foldername)
{
    QString accountHash = QString(QCryptographicHash::hash((m_Email.toUtf8()), QCryptographicHash::Md5).toHex());
    QString cachePath = QDir::homePath() + "/.config/Pelipper/" + accountHash;
    QDir dir(cachePath + "/MAILS");

    if (!dir.exists()) {
        return nullptr;
    }

    m_Settings->beginGroup("uids");
    QStringList uids = m_Settings->value(foldername, QStringList()).toStringList();
    m_Settings->endGroup();

    QList<Message *> *msgList = new QList<Message *>();

    QFileInfoList filenameList = dir.entryInfoList(uids);

    for (const auto &fileinfo : qAsConst(filenameList)) {
        QFile file(fileinfo.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return nullptr;

        QTextStream in(&file);
        QString data;
        while (!in.atEnd()) {
            QString line = in.readAll();
            data.append(line);
        }

        Message *msg = new Message(m_Email, fileinfo.fileName().toLong());
        msg->parseFromData(data);
        msgList->append(msg);
    }

    return msgList;
}

Message *ImapCache::getMessage(const QString &foldername, ssize_t uid, QString &data)
{
    QDir dir(m_CacheFolderPath + "/MAILS");

    if (!dir.exists()) {
        return nullptr;
    }

    QFileInfoList fileinfoList = dir.entryInfoList(QStringList() << QString::number(uid));

    if (fileinfoList.isEmpty()) {
        return nullptr;
    }

    QFileInfo fileinfo = fileinfoList.at(0);

    QFile file(fileinfo.absoluteFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return nullptr;

    Message *msg = new Message(m_Email, uid);
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readAll();
        data.append(line);
    }

    msg->parseFromData(data);

    return msg;
}

bool ImapCache::insertAllMessages(const QString &foldername, QList<Message *> *msgList)
{
    QDir dir(m_CacheFolderPath + "/MAILS");

    if (!dir.exists()) {
        bool result = dir.mkpath(m_CacheFolderPath + "/MAILS");

        if (!result) {
            return false;
        }
    }

    QStringList uidList;

    for (int i = 0; i < msgList->length(); i++) {
        auto msg = msgList->at(i);
        uidList.append(QString::number(msg->uid()));

        QFile file(dir.path() + "/" + QString::number(msg->uid()));
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return false;

        QTextStream out(&file);
        out << msg->Data();
    }

    m_Settings->beginGroup("uids");
    m_Settings->setValue(foldername, uidList);
    m_Settings->endGroup();
    return true;
}

bool ImapCache::insertMessage(const QString &foldername, Message *msg)
{
    QDir dir(m_CacheFolderPath + "/MAILS");

    if (!dir.exists()) {
        bool result = dir.mkpath(m_CacheFolderPath + "/MAILS");

        if (!result) {
            return false;
        }
    }

    QFile file(dir.path() + "/" + QString::number(msg->uid()));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << msg->Data();

    QStringList uidList;

    m_Settings->beginGroup("uids");
    uidList = m_Settings->value(foldername, QStringList()).toStringList();
    uidList.append(QString::number(msg->uid()));
    m_Settings->setValue(foldername, uidList);
    m_Settings->endGroup();

    return true;
}

bool ImapCache::deleteMessage(const QString &foldername, QList<ssize_t> uidList)
{
    QDir dir(m_CacheFolderPath + "/MAILS");

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

    for (const auto &fileinfo : qAsConst(fileinfoList)) {
        QFile file(fileinfo.absoluteFilePath());
        if (!file.remove()) {
            return false;
        }
    }

    return true;
}

QString ImapCache::getParentFolderName(const QString &fullfoldername, const QChar &delimter)
{
    int lastIndex = fullfoldername.lastIndexOf(delimter);
    if (lastIndex != -1) {
        return fullfoldername.left(lastIndex);
    }

    return "";
}
