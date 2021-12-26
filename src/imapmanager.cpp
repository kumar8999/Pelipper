#include "imapmanager.h"

ImapManager::ImapManager(const QString &email, const QString &password, const QString &server, int port, QObject *parent)
    : m_Email(email)
    , m_Password(password)
    , m_Server(server)
    , m_Port(port)
    , m_ThreadIsRunning(false)
    , QObject(parent)
{
    m_FolderList = new QList<Folder *>();
    m_Imap = new Imap(email, password, server, port);
}

void ImapManager::login()
{
    if (!m_Imap->connect()) {
        emit connectedToServer(false);
        return;
    } else {
        emit connectedToServer(true);
    }

    if (!m_Imap->login()) {
        emit loggedIn(false);
    } else {
        emit loggedIn(true);
    }
}

void ImapManager::getFolders()
{
    Request request;
    request.cmd = FOLDER;
    m_Request.enqueue(request);
}

void ImapManager::getAllHeaders(const QString &foldername)
{
    Request request;
    request.cmd = HEADERS;
    request.foldername = foldername;
    m_Request.enqueue(request);
}

void ImapManager::getBody(ssize_t uid)
{
    Request request;
    request.cmd = MSG;
    request.uid = uid;
    m_Request.enqueue(request);
}

void ImapManager::startProcess()
{
    while (m_ThreadIsRunning) {
        if (!m_Request.isEmpty()) {
            Request request = m_Request.dequeue();
            processRequest(request);
        }
    }
}

void ImapManager::processRequest(Request request)
{
    switch (request.cmd) {
    case FOLDER:
        readFolders();
        break;
    case HEADERS:
        readAllHeaders(request.foldername);
        break;
    case MSG:
        readBody(request.uid);
        break;
    }
}

void ImapManager::readFolders()
{
    QStringList folderList;
    if (m_Imap->checkInternet()) {
        m_Imap->getFolders("*", folderList);
    } else {
        // load from cache
    }

    if (!folderList.isEmpty()) {
        parseFolders(folderList);
        emit readFoldersFinished(m_FolderList);
    }
}

void ImapManager::parseFolders(QStringList folderList)
{
    for (int i = 0; i < folderList.length(); i++) {
        parseFolder(folderList.at(i).split("/"), 0);
    }
}

void ImapManager::parseFolder(QStringList foldernameList, int index)
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

QString ImapManager::getFolderName(QStringList folderList, int index)
{
    QString foldername = "";
    for (int i = 0; i <= index; i++) {
        foldername += folderList.at(i) + "/";
    }
    int lastSeperatorPos = foldername.lastIndexOf("/");
    return foldername.left(lastSeperatorPos);
}

QString ImapManager::getParentFolderName(QStringList folderList, int index)
{
    QString foldername = "";
    for (int i = 0; i < index; i++) {
        foldername += folderList.at(i) + "/";
    }
    int lastSeperatorPos = foldername.lastIndexOf("/");
    return foldername.left(lastSeperatorPos);
}

void ImapManager::readAllHeaders(QString foldername)
{
    QList<ssize_t> nonCachedUidList;
    QMap<ssize_t, QString> data;
    if (m_Imap->checkInternet()) {
        m_Imap->getAllHeaders(foldername, nonCachedUidList, data);
    }

    QList<Message *> *msgList = new QList<Message *>();
    QMapIterator<ssize_t, QString> iter(data);
    while (iter.hasNext()) {
        iter.next();

        Message *msg = new Message(m_Email, iter.key());
        msg->setHeaderData(iter.value());
        msgList->append(msg);

        if (msgList->length() == 30) {
            emit readAllHeadersFinished(msgList);
            msgList->clear();
        }
    }
}

void ImapManager::readBody(ssize_t uid)
{
    QString data;
    if (m_Imap->checkInternet()) {
        m_Imap->getBody(uid, data);
    }

    Message *msg = new Message(m_Email, uid);
    msg->setBodyData(data);
    emit readBodyFinished(msg);
}
