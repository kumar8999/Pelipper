#include "imapcache.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

ImapCache::ImapCache(const QString &email, QObject *parent)
    : m_Email(email)
    , QObject(parent)
{
    QString accountHash = QString(QCryptographicHash::hash((m_Email.toUtf8()), QCryptographicHash::Md5).toHex());
    QString cacheFolderPath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

    if (cacheFolderPath == "") {
        cacheFolderPath = QDir::homePath() + "/.cache/Pelipper/";
    } else {
        cacheFolderPath += "/Pelipper";
    }

    cacheFolderPath = cacheFolderPath + "/" + accountHash;

    QDir dir;

    if (dir.mkpath(cacheFolderPath)) {
        m_cacheFilePath = cacheFolderPath + "/cache";
    }
}

QList<Folder *> *ImapCache::getFolders()
{
    QFile file(m_cacheFilePath);

    QList<Folder *> *folderList = new QList<Folder *>();

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return folderList;
    }

    QJsonObject mainObj = QJsonDocument().fromJson(file.readAll()).object();
    QJsonArray folders = mainObj["folders"].toArray();

    for (auto folder : folders) {
        folderList->append(getFolderObj(folder.toObject()));
    }

    return folderList;
}

QStringList ImapCache::fetchFoldersStr()
{
    QFile file(m_cacheFilePath);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return QStringList();
    }

    QJsonObject mainObj = QJsonDocument().fromJson(file.readAll()).object();
    QJsonArray folderListArr = mainObj["foldersList"].toArray();

    QStringList folderList;
    for (const auto &folder : qAsConst(folderListArr)) {
        folderList.append(folder.toString());
    }

    return folderList;
}

bool ImapCache::insertFoldersStr(const QStringList &folderList)
{
    QFile file(m_cacheFilePath);

    if (!file.open(QIODevice::ReadWrite)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QJsonObject mainObj = QJsonDocument().fromJson(file.readAll()).object();
    QJsonArray folderListArr;

    for (const auto &folder : folderList) {
        folderListArr.append(folder);
    }

    mainObj.insert("foldersList", folderListArr);

    return file.write(QJsonDocument(mainObj).toJson());
}

bool ImapCache::insertFolders(const QList<Folder *> &folderList)
{
    QFile file(m_cacheFilePath);

    if (!file.open(QIODevice::ReadWrite)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QJsonObject mainObj = QJsonDocument().fromJson(file.readAll()).object();
    QJsonArray folders;

    for (int i = 0; i < folderList.length(); i++) {
        folders.append(insertFolderObj(folderList.at(i)));
    }

    mainObj.insert("folders", folders);

    return file.write(QJsonDocument(mainObj).toJson());
}

void ImapCache::insertUidList(const QString &foldername, const QList<ssize_t> &uidList)
{
    QFile file(m_cacheFilePath);

    if (!file.open(QIODevice::ReadWrite)) {
        qWarning("Couldn't open save file.");
        return;
    }

    QJsonArray uids;
    for (const auto &uid : qAsConst(uidList)) {
        uids.append(QString::number(uid));
    }

    QJsonObject uidObj;
    uidObj[foldername] = uids;

    QJsonObject mainObj = QJsonDocument().fromJson(file.readAll()).object();
    mainObj.insert("uids", uidObj);

    file.write(QJsonDocument(mainObj).toJson());
}

QList<ssize_t> ImapCache::getUidList(const QString &foldername)
{
    QFile file(m_cacheFilePath);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return QList<ssize_t>();
    }

    QJsonObject mainObj = QJsonDocument().fromJson(file.readAll()).object();
    QJsonObject uidObj = mainObj["uids"].toObject();
    QJsonArray uids = uidObj[foldername].toArray();

    QList<ssize_t> uidList;
    for (const auto &uid : qAsConst(uids)) {
        uidList.append(uid.toInt());
    }

    return uidList;
}

QList<Message *> *ImapCache::getAllMessages(const QString &foldername)
{
    QString accountHash = QString(QCryptographicHash::hash((m_Email.toUtf8()), QCryptographicHash::Md5).toHex());
    QString cacheFolderPath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

    if (cacheFolderPath == "") {
        cacheFolderPath = QDir::homePath() + "/.cache/Pelipper/";
    } else {
        cacheFolderPath += "/Pelipper";
    }

    cacheFolderPath = cacheFolderPath + "/" + accountHash;

    QDir dir(cacheFolderPath + "/MAILS");

    if (!dir.exists()) {
        qDebug() << " dir not exists";
        return new QList<Message *>();
    }

    QList<ssize_t> uidList = getUidList(foldername);
    QStringList uids;
    for (const auto &uid : qAsConst(uidList)) {
        uids.append(QString::number(uid));
    }

    QList<Message *> *msgList = new QList<Message *>();

    QFileInfoList filenameList = dir.entryInfoList(uids);

    for (const auto &fileinfo : qAsConst(filenameList)) {
        QFile file(fileinfo.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            continue;
        }

        QTextStream in(&file);
        QString data;
        while (!in.atEnd()) {
            QString line = in.readAll();
            data.append(line);
        }

        Message *msg = new Message(m_Email, fileinfo.fileName().toLong());
        msg->parseFromData(data);
        msg->setFlags(getFlags(fileinfo.fileName().toLong()));
        msgList->append(msg);
    }

    return msgList;
}

Message *ImapCache::getMessage(const QString &foldername, ssize_t uid, QString &data)
{
    QString accountHash = QString(QCryptographicHash::hash((m_Email.toUtf8()), QCryptographicHash::Md5).toHex());
    QString cacheFolderPath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

    if (cacheFolderPath == "") {
        cacheFolderPath = QDir::homePath() + "/.cache/Pelipper/";
    } else {
        cacheFolderPath += "/Pelipper";
    }

    cacheFolderPath = cacheFolderPath + "/" + accountHash;

    QDir dir(cacheFolderPath + "/MAILS");
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

    Flags *flag = getFlags(uid);
    msg->setFlags(flag);

    return msg;
}

Flags *ImapCache::getFlags(const ssize_t &uid)
{
    QFile file(m_cacheFilePath);

    if (!file.open(QIODevice::ReadWrite)) {
        qWarning("Couldn't open save file.");
        return nullptr;
    }

    Flags *flag = new Flags();

    QJsonObject mainObj = QJsonDocument().fromJson(file.readAll()).object();
    QJsonObject flagObj = mainObj[QString::number(uid)].toObject();

    flag->setSeenFlag(flagObj["seen"].toBool());
    flag->setDeletedFlag(flagObj["deleted"].toBool());
    flag->setAnsweredFlag(flagObj["answered"].toBool());
    flag->setDraftFlag(flagObj["draft"].toBool());
    flag->setRecentFlag(flagObj["recent"].toBool());

    return flag;
}

bool ImapCache::insertAllMessages(const QString &foldername, QList<Message *> *msgList)
{
    QString accountHash = QString(QCryptographicHash::hash((m_Email.toUtf8()), QCryptographicHash::Md5).toHex());
    QString cacheFolderPath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

    if (cacheFolderPath == "") {
        cacheFolderPath = QDir::homePath() + "/.cache/Pelipper/";
    } else {
        cacheFolderPath += "/Pelipper";
    }

    cacheFolderPath = cacheFolderPath + "/" + accountHash;

    QDir dir(cacheFolderPath + "/MAILS");

    if (!dir.exists()) {
        bool result = dir.mkpath(cacheFolderPath + "/MAILS");

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

        saveFlags(msg->uid(), msg->flags());
    }

    return true;
}

bool ImapCache::insertMessage(const QString &foldername, Message *msg)
{
    QString accountHash = QString(QCryptographicHash::hash((m_Email.toUtf8()), QCryptographicHash::Md5).toHex());
    QString cacheFolderPath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

    if (cacheFolderPath == "") {
        cacheFolderPath = QDir::homePath() + "/.cache/Pelipper/";
    } else {
        cacheFolderPath += "/Pelipper";
    }

    cacheFolderPath = cacheFolderPath + "/" + accountHash;

    QDir dir(cacheFolderPath + "/MAILS");

    if (!dir.exists()) {
        bool result = dir.mkpath(cacheFolderPath + "/MAILS");

        if (!result) {
            return false;
        }
    }

    QFile file(dir.path() + "/" + QString::number(msg->uid()));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << msg->Data();

    return true;
}

bool ImapCache::saveFlags(const ssize_t &uid, Flags *flags)
{
    QFile file(m_cacheFilePath);

    if (!file.open(QIODevice::ReadWrite)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QJsonObject mainObj = QJsonDocument().fromJson(file.readAll()).object();
    QJsonObject flagObj = mainObj[QString::number(uid)].toObject();

    flagObj.insert("seen", flags->seenFlag());
    flagObj.insert("deleted", flags->deletedFlag());
    flagObj.insert("answered", flags->answeredFlag());
    flagObj.insert("draft", flags->draftFlag());
    flagObj.insert("recent", flags->recentFlag());

    mainObj.insert(QString::number(uid), flagObj);

    return file.write(QJsonDocument(mainObj).toJson());
}

bool ImapCache::deleteMessage(const QString &foldername, QList<ssize_t> uidList)
{
    QString accountHash = QString(QCryptographicHash::hash((m_Email.toUtf8()), QCryptographicHash::Md5).toHex());
    QString cacheFolderPath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

    if (cacheFolderPath == "") {
        cacheFolderPath = QDir::homePath() + "/.cache/Pelipper/";
    } else {
        cacheFolderPath += "/Pelipper";
    }

    cacheFolderPath = cacheFolderPath + "/" + accountHash;

    QDir dir(cacheFolderPath + "/MAILS");

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

QJsonObject ImapCache::insertFolderObj(Folder *folder)
{
    QJsonObject obj;

    obj["foldername"] = folder->FullName();
    obj["delimiter"] = QJsonValue(folder->Delimiter());

    if (folder->hasChildren()) {
        QJsonArray folderArr;
        for (auto childFolder : folder->Children()) {
            folderArr.append(insertFolderObj(childFolder));
        }
        obj["children"] = folderArr;
    }

    return obj;
}

Folder *ImapCache::getFolderObj(QJsonObject obj)
{
    QString foldername = obj["foldername"].toString();
    QChar delimiter = obj["delimiter"].toVariant().toChar();
    Folder *folder = new Folder(foldername, delimiter);

    if (obj.contains("children")) {
        QJsonArray folderArr = obj["children"].toArray();

        for (auto childFolder : qAsConst(folderArr)) {
            Folder *child = getFolderObj(childFolder.toObject());
            folder->appendChild(child);
        }
    }

    return folder;
}
