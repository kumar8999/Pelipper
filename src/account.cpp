#include "account.h"

#include <QDebug>
#include <QtConcurrent>

Account::Account(const QString &username,
                 const QString &email,
                 const QString &password,
                 const QString &imapServer,
                 const int &imapPort,
                 const QString &smtpServer,
                 const int &smtpPort,
                 QObject *parent)
    : m_Username(username)
    , m_Email(email)
    , m_Password(password)
    , m_ImapServer(imapServer)
    , m_ImapPort(imapPort)
    , m_SmtpServer(smtpServer)
    , m_SmtpPort(smtpPort)
    , m_SelectFolder("")
    , QObject(parent)
{
    m_FolderList = new QList<Folder *>();

    m_Imap = new Imap(email, password, imapServer, imapPort);
    m_ImapCache = new ImapCache(email, this);

    bool result = m_Imap->connect();
    result = m_Imap->login();

    QtConcurrent::run(this, &Account::startCacheThread);
}

Account::~Account()
{
    delete m_Imap;
}

bool Account::getFolders()
{
    bool result;
    QStringList folderList;

    if (m_Imap->checkInternet()) {
        result = m_Imap->getFolders("*", folderList);
        if (result) {
            m_ImapCache->insertFolders(folderList);
        }
    } else {
        result = m_ImapCache->getFolders(folderList);
    }

    parseFolders(folderList);

    emit folderReadFinished(m_FolderList);
    return result;
}

bool Account::selectFolder(QString foldername)
{
    bool result;

    m_SelectFolder = foldername;

    result = m_Imap->selectFolder(foldername);

    emit folderSelected(foldername);

    return result;
}

bool Account::getHeaders(QString foldername, QMap<ssize_t, QString> &headerList)
{
    QList<ssize_t> cacheUidList;

    bool result;

    result = m_ImapCache->getUidList(foldername, cacheUidList);

    if (!result)
        cacheUidList.clear();

    result = m_ImapCache->getAllMessages(foldername, headerList);

    if (!result) {
        headerList.clear();
    }

    QList<ssize_t> nonCacheUidList;

    result = m_Imap->getUids(foldername, nonCacheUidList);

    for (auto uid : qAsConst(cacheUidList)) {
        nonCacheUidList.removeAll(uid);
    }

    result = m_Imap->getAllHeaders(foldername, nonCacheUidList, headerList);

    return true;
}

bool Account::getBody(ssize_t uid, QString &data)
{
    bool result = m_ImapCache->getBody(m_SelectFolder, uid, data);

    if (!result) {
        result = m_Imap->getBody(uid, data);
    }

    return result;
}

const QString &Account::Email() const
{
    return m_Email;
}

bool Account::deleteMessage(QList<ssize_t> uidList)
{
    bool result = m_Imap->deleteMessage(m_SelectFolder, uidList);

    qDebug() << "delete " << result;

    if (result) {
        m_ImapCache->deleteMessage(m_SelectFolder, uidList);
    }

    return result;
}

bool Account::moveMessage(const QString &sourceFoldername, const QString &destFolderName, QList<ssize_t> uidList)
{
    bool result = m_Imap->moveMessage(sourceFoldername, destFolderName, uidList);

    return result;
}

void Account::startCacheThread()
{
    Imap *imap = new Imap(m_Email, m_Password, m_ImapServer, m_ImapPort);

    bool result = imap->connect();

    if (result) {
        result = imap->login();
    }

    QStringList folderList;

    result = imap->getFolders("*", folderList);

    if (!result) {
        return;
    }

    for (const auto &foldername : qAsConst(folderList)) {
        QMap<ssize_t, QString> msgData;

        QList<ssize_t> cacheUidList;
        QList<ssize_t> nonCacheUidList;

        result = imap->selectFolder(foldername);
        result = m_ImapCache->getUidList(foldername, cacheUidList);
        result = imap->getUids(foldername, nonCacheUidList);

        if (nonCacheUidList.isEmpty()) {
            continue;
        }

        for (auto uid : qAsConst(cacheUidList)) {
            nonCacheUidList.removeAll(uid);
        }

        result = imap->getAllBody(msgData, nonCacheUidList);

        if (result) {
            m_ImapCache->insertMessages(foldername, msgData);
        }
    }
}

void Account::parseFolders(QStringList folderList)
{
    m_FolderList->clear();
    m_Folders.clear();

    for (int i = 0; i < folderList.length(); i++) {
        parseFolder(folderList.at(i).split("/"), 0);
    }
}

void Account::parseFolder(QStringList foldernameList, int index)
{
    QString folderName = getFolderName(foldernameList, index);

    Folder *item = m_Folders[folderName];

    if (item == nullptr) {
        QString parentFolderName = getParentFolderName(foldernameList, index);
        Folder *parentItem = m_Folders[parentFolderName];
        item = new Folder(foldernameList.at(index), folderName);
        item->setEmail(m_Email);

        if (parentItem == nullptr) {
            m_FolderList->append(item);
        } else {
            parentItem->appendChild(item);
        }
        m_Folders[folderName] = item;
        if (foldernameList.length() - 1 != index) {
            index = index + 1;
            parseFolder(foldernameList, index);
        }
    } else {
        if (foldernameList.length() - 1 != index) {
            index = index + 1;
            parseFolder(foldernameList, index);
        }
    }
}

QString Account::getFolderName(QStringList folderList, int index)
{
    QString foldername = "";
    for (int i = 0; i <= index; i++) {
        foldername += folderList.at(i) + "/";
    }
    int lastSeperatorPos = foldername.lastIndexOf("/");
    return foldername.left(lastSeperatorPos);
}

QString Account::getParentFolderName(QStringList folderList, int index)
{
    QString foldername = "";
    for (int i = 0; i < index; i++) {
        foldername += folderList.at(i) + "/";
    }
    int lastSeperatorPos = foldername.lastIndexOf("/");
    return foldername.left(lastSeperatorPos);
}
