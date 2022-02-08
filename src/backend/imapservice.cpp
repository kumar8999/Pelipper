#include "imapservice.h"

#include <QDebug>

ImapService::ImapService(const QString &email, const QString &password, const QString &server, int port)
    : m_Email(email)
    , m_Password(password)
    , m_Server(server)
    , m_Port(port)
    , m_Connected(false)
    , m_LoggedIn(false)
{
    m_ImapCache = new ImapCache(email, this);
    m_Imap = mailimap_new(0, NULL);
}

ImapService::~ImapService()
{
    delete m_ImapCache;
    mailimap_free(m_Imap);
}

bool ImapService::connect()
{
    QMutexLocker locker(&m_Mutex);

    int r = mailimap_ssl_connect(m_Imap, m_Server.toStdString().c_str(), m_Port);

    if (r == MAILIMAP_NO_ERROR_NON_AUTHENTICATED) {
        m_Connected = true;
    }

    return m_Connected;
}

bool ImapService::login()
{
    QMutexLocker locker(&m_Mutex);

    int r = mailimap_login(m_Imap, m_Email.toStdString().c_str(), m_Password.toStdString().c_str());

    if (r == MAILIMAP_NO_ERROR) {
        m_LoggedIn = true;
    }

    return m_LoggedIn;
}

bool ImapService::logout()
{
    QMutexLocker locker(&m_Mutex);

    int r = mailimap_logout(m_Imap);

    return r == MAILIMAP_NO_ERROR;
}

bool ImapService::selectFolder(QString folderName)
{
    int r = mailimap_select(m_Imap, folderName.toStdString().c_str());

    m_SelectedFolder = folderName;

    return r == MAILIMAP_NO_ERROR;
}

QList<Folder *> *ImapService::getFolders(QString folderName)
{
    QMutexLocker locker(&m_Mutex);

    clist *list = NULL;

    if (!checkInternet() || (!m_Connected && !m_LoggedIn)) {
        return m_ImapCache->getFolders();
    }

    int r = mailimap_list(m_Imap, "", folderName.toStdString().c_str(), &list);

    if (r != MAILIMAP_NO_ERROR) {
        qDebug() << "Could not fetch folder list";
        qDebug() << "Errorcode: " << r;
        return nullptr;
    }

    Folder *_parent = nullptr;
    QMap<QString, mailimap_mailbox_list *> folderMap;

    for (clistiter *it = clist_begin(list); it != NULL; it = it->next) {
        mailimap_mailbox_list *mblist = (mailimap_mailbox_list *)clist_content(it);
        if (mblist) {
            mailimap_mbx_list_flags *bflags = mblist->mb_flag;
            if (bflags) {
                const std::string &foldername = std::string(mblist->mb_name);
                folderMap[QString::fromStdString(foldername)] = mblist;
            }
        }
    }

    QList<Folder *> *folderList = new QList<Folder *>();
    QMapIterator<QString, mailimap_mailbox_list *> folderMapIter(folderMap);
    QMap<QString, Folder *> folders;

    while (folderMapIter.hasNext()) {
        folderMapIter.next();

        QString foldername = folderMapIter.key();
        mailimap_mailbox_list *mblist = folderMapIter.value();
        QChar delimiter = QChar(mblist->mb_delimiter);
        QString folderparentName = getParentFolderName(foldername, delimiter);

        Folder *folder = new Folder(foldername, delimiter);
        folders.insert(foldername, folder);

        if (folderparentName != "") {
            Folder *parentFolder = folders.value(folderparentName);
            parentFolder->appendChild(folder);
            continue;
        }

        folderList->append(folder);
    }

    mailimap_list_result_free(list);

    m_ImapCache->insertFolders(*folderList);

    return folderList;
}

QList<ssize_t> ImapService::getNonCachedUids(const QString &foldername)
{
    int r;

    mailimap_set *set = mailimap_set_new_interval(0, 1);
    clist *fetchResult = NULL;
    mailimap_fetch_type *fetchType = mailimap_fetch_type_new_fetch_att_list_empty();
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, mailimap_fetch_att_new_uid());

    r = mailimap_uid_fetch(m_Imap, set, fetchType, &fetchResult);

    if (r != MAILIMAP_NO_ERROR) {
        qDebug() << "get uids";
        qDebug() << "Errorcode: " << r;
        return QList<ssize_t>();
    }

    QList<ssize_t> uidList;
    QList<ssize_t> nonCachedUidList;

    for (clistiter *it = clist_begin(fetchResult); it != NULL; it = clist_next(it)) {
        mailimap_msg_att *msgAtt = (mailimap_msg_att *)clist_content(it);

        uint32_t uid = 0;

        for (clistiter *cur = clist_begin(msgAtt->att_list); cur != NULL; cur = clist_next(cur)) {
            mailimap_msg_att_item *item = (mailimap_msg_att_item *)clist_content(cur);

            if (item->att_type == MAILIMAP_MSG_ATT_ITEM_DYNAMIC) {
                continue;
            }

            if (item->att_type == MAILIMAP_MSG_ATT_ITEM_STATIC) {
                if (item->att_data.att_static->att_type == MAILIMAP_MSG_ATT_UID) {
                    uid = item->att_data.att_static->att_data.att_uid;
                    uidList.append(uid);
                } else {
                    continue;
                }
            }
        }

        if (uid == 0) {
            continue;
        }

        uidList.append(uid);
    }

    QList<ssize_t> cachedUidList;
    cachedUidList = m_ImapCache->getUidList(foldername);

    QSet<ssize_t> cachedUidListSet = QSet<ssize_t>(cachedUidList.begin(), cachedUidList.end());
    QSet<ssize_t> uidListSet = QSet<ssize_t>(uidList.begin(), uidList.end());

    QSet<ssize_t> nonCachedUidListSet = uidListSet.subtract(cachedUidListSet);

    nonCachedUidList = QList<ssize_t>(nonCachedUidListSet.begin(), nonCachedUidListSet.end());

    mailimap_fetch_list_free(fetchResult);
    mailimap_fetch_type_free(fetchType);
    mailimap_set_free(set);

    return nonCachedUidList;
}

QList<Message *> *ImapService::getAllHeaders(const QString &foldername)
{
    QMutexLocker locker(&m_Mutex);

    if (!selectFolder(foldername)) {
        return nullptr;
    }

    QList<ssize_t> nonCachedUidList = getNonCachedUids(foldername);

    QList<Message *> *msgList;

    msgList = m_ImapCache->getAllMessages(foldername);

    if (msgList == nullptr) {
        msgList = new QList<Message *>();
    }

    if (nonCachedUidList.isEmpty()) {
        return msgList;
    }

    mailimap_set *set = mailimap_set_new_interval(0, 1);
    clist *fetchResult = NULL;
    mailimap_fetch_type *fetchType = mailimap_fetch_type_new_fetch_att_list_empty();
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, mailimap_fetch_att_new_rfc822_header());
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, mailimap_fetch_att_new_uid());
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, mailimap_fetch_att_new_flags());

    if (!nonCachedUidList.isEmpty()) {
        set = mailimap_set_new_empty();
        for (auto uid : qAsConst(nonCachedUidList)) {
            mailimap_set_add_single(set, uid);
        }
    }

    int r = mailimap_uid_fetch(m_Imap, set, fetchType, &fetchResult);

    if (r != MAILIMAP_NO_ERROR) {
        qDebug() << "headers code: " << r;
        return nullptr;
    }

    for (clistiter *it = clist_begin(fetchResult); it != NULL; it = clist_next(it)) {
        mailimap_msg_att *msgAtt = (mailimap_msg_att *)clist_content(it);

        QString data;
        Flags *flags = new Flags();
        ssize_t uid = 0;

        for (clistiter *cur = clist_begin(msgAtt->att_list); cur != NULL; cur = clist_next(cur)) {
            mailimap_msg_att_item *item = (mailimap_msg_att_item *)clist_content(cur);

            if (item->att_type == MAILIMAP_MSG_ATT_ITEM_DYNAMIC) {
                if (item->att_data.att_dyn->att_list != NULL) {
                    for (clistiter *dit = clist_begin(item->att_data.att_dyn->att_list); dit != NULL; dit = clist_next(dit)) {
                        mailimap_flag_fetch *flagFetch = (mailimap_flag_fetch *)clist_content(dit);
                        if (flagFetch && flagFetch->fl_type) {
                            switch (flagFetch->fl_type) {
                            case MAILIMAP_FLAG_FETCH_RECENT:
                                flags->setRecentFlag(true);
                                break;
                            }
                        }
                        if (flagFetch && flagFetch->fl_flag) {
                            switch (flagFetch->fl_flag->fl_type) {
                            case MAILIMAP_FLAG_SEEN:
                                flags->setSeenFlag(true);
                                break;

                            case MAILIMAP_FLAG_DELETED:
                                flags->setDeletedFlag(true);
                                break;

                            case MAILIMAP_FLAG_ANSWERED:
                                flags->setAnsweredFlag(true);
                                break;

                            case MAILIMAP_FLAG_FLAGGED:
                                break;

                            case MAILIMAP_FLAG_DRAFT:
                                flags->setDraftFlag(true);
                                break;

                            case MAILIMAP_FLAG_KEYWORD:
                                break;

                            case MAILIMAP_FLAG_EXTENSION:
                                break;
                            default:
                                break;
                            }
                        }
                    }
                }
            }

            if (item->att_type == MAILIMAP_MSG_ATT_ITEM_STATIC) {
                if (item->att_data.att_static->att_type == MAILIMAP_MSG_ATT_RFC822_HEADER) {
                    std::string hData = std::string(item->att_data.att_static->att_data.att_rfc822_header.att_content,
                                                    item->att_data.att_static->att_data.att_rfc822_header.att_length);
                    data = QString::fromStdString(hData);

                } else if (item->att_data.att_static->att_type == MAILIMAP_MSG_ATT_UID) {
                    uid = item->att_data.att_static->att_data.att_uid;
                }
            }
        }

        if (uid == 0) {
            continue;
        }

        Message *msg = new Message(m_Email, uid);
        msg->setHeaderData(data);
        msg->setFlags(flags);
        msgList->append(msg);
    }

    mailimap_fetch_list_free(fetchResult);
    mailimap_fetch_type_free(fetchType);
    mailimap_set_free(set);

    return msgList;
}

Message *ImapService::getBody(ssize_t uid)
{
    QMutexLocker locker(&m_Mutex);

    mailimap_set *set;
    mailimap_section *section;
    mailimap_fetch_type *fetchType;
    mailimap_fetch_att *fetchAtt;
    clist *fetchResult = NULL;
    int r;

    section = mailimap_section_new(NULL);
    set = mailimap_set_new_single(uid);
    fetchType = mailimap_fetch_type_new_fetch_att_list_empty();
    fetchAtt = mailimap_fetch_att_new_body_section(section);
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, mailimap_fetch_att_new_uid());
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, fetchAtt);
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, mailimap_fetch_att_new_flags());

    r = mailimap_uid_fetch(m_Imap, set, fetchType, &fetchResult);

    if (r != MAILIMAP_NO_ERROR) {
        qDebug() << "body code: " << r;
        return nullptr;
    }
    QString data;
    Flags *flags = new Flags();

    for (clistiter *it = clist_begin(fetchResult); it != NULL; it = clist_next(it)) {
        mailimap_msg_att *msg_att = (mailimap_msg_att *)clist_content(it);

        for (clistiter *cur = clist_begin(msg_att->att_list); cur != NULL; cur = clist_next(cur)) {
            mailimap_msg_att_item *item = (mailimap_msg_att_item *)clist_content(cur);

            if (item->att_type == MAILIMAP_MSG_ATT_ITEM_DYNAMIC) {
                if (item->att_data.att_dyn->att_list != NULL) {
                    for (clistiter *dit = clist_begin(item->att_data.att_dyn->att_list); dit != NULL; dit = clist_next(dit)) {
                        mailimap_flag_fetch *flagFetch = (mailimap_flag_fetch *)clist_content(dit);
                        if (flagFetch && flagFetch->fl_type) {
                            switch (flagFetch->fl_type) {
                            case MAILIMAP_FLAG_FETCH_RECENT:
                                flags->setRecentFlag(true);
                                break;
                            }
                        }
                        if (flagFetch && flagFetch->fl_flag) {
                            switch (flagFetch->fl_flag->fl_type) {
                            case MAILIMAP_FLAG_SEEN:
                                flags->setSeenFlag(true);
                                break;

                            case MAILIMAP_FLAG_DELETED:
                                flags->setDeletedFlag(true);
                                break;

                            case MAILIMAP_FLAG_ANSWERED:
                                flags->setAnsweredFlag(true);
                                break;

                            case MAILIMAP_FLAG_FLAGGED:
                                break;

                            case MAILIMAP_FLAG_DRAFT:
                                flags->setDraftFlag(true);
                                break;

                            case MAILIMAP_FLAG_KEYWORD:
                                break;

                            case MAILIMAP_FLAG_EXTENSION:
                                break;
                            default:
                                break;
                            }
                        }
                    }
                }
            }

            if (item->att_type == MAILIMAP_MSG_ATT_ITEM_STATIC) {
                if (item->att_data.att_static->att_type == MAILIMAP_MSG_ATT_BODY_SECTION) {
                    std::string d(item->att_data.att_static->att_data.att_body_section->sec_body_part,
                                  item->att_data.att_static->att_data.att_body_section->sec_length);
                    data = QString::fromStdString(d);
                }

                if (item->att_data.att_static->att_type == MAILIMAP_MSG_ATT_UID) {
                    uid = item->att_data.att_static->att_data.att_uid;
                }
            }
        }

        if (uid == 0) {
            continue;
        }

        if (data.isEmpty()) {
            continue;
        }
    }

    Message *msg = new Message(uid);
    msg->setFlags(flags);
    msg->setBodyData(data);

    m_ImapCache->insertMessage(m_SelectedFolder, msg);

    mailimap_fetch_list_free(fetchResult);
    mailimap_fetch_type_free(fetchType);
    mailimap_set_free(set);

    return msg;
}

QList<Message *> *ImapService::getAllMessage(const QString &foldername)
{
    QMutexLocker locker(&m_Mutex);

    mailimap_set *set;
    mailimap_section *section;
    mailimap_fetch_type *fetchType;
    mailimap_fetch_att *fetchAtt;
    clist *fetchResult = NULL;

    QList<ssize_t> nonCachedUids = getNonCachedUids(foldername);

    QList<Message *> *msgList = new QList<Message *>();

    msgList = m_ImapCache->getAllMessages(foldername);

    if (!checkInternet() || (!m_Connected && !m_LoggedIn)) {
        return msgList;
    }

    if (msgList == nullptr) {
        msgList = new QList<Message *>();
    }

    if (!selectFolder(foldername)) {
        return nullptr;
    }

    section = mailimap_section_new(NULL);
    set = mailimap_set_new_interval(0, 1);
    fetchType = mailimap_fetch_type_new_fetch_att_list_empty();
    fetchAtt = mailimap_fetch_att_new_body_peek_section(section);
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, mailimap_fetch_att_new_uid());
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, fetchAtt);

    if (!nonCachedUids.isEmpty()) {
        set = mailimap_set_new_empty();
    } else {
        for (auto uid : qAsConst(nonCachedUids)) {
            mailimap_set_add_single(set, uid);
        }
    }

    int r = mailimap_uid_fetch(m_Imap, set, fetchType, &fetchResult);

    if (r != MAILIMAP_NO_ERROR) {
        qDebug() << "body code: " << r;
        return nullptr;
    }

    for (clistiter *it = clist_begin(fetchResult); it != NULL; it = clist_next(it)) {
        mailimap_msg_att *msg_att = (mailimap_msg_att *)clist_content(it);

        QString data;
        ssize_t uid = 0;

        for (clistiter *cur = clist_begin(msg_att->att_list); cur != NULL; cur = clist_next(cur)) {
            mailimap_msg_att_item *item = (mailimap_msg_att_item *)clist_content(cur);

            if (item->att_type == MAILIMAP_MSG_ATT_ITEM_DYNAMIC)
                continue;

            if (item->att_type == MAILIMAP_MSG_ATT_ITEM_STATIC) {
                if (item->att_data.att_static->att_type == MAILIMAP_MSG_ATT_BODY_SECTION) {
                    std::string d(item->att_data.att_static->att_data.att_body_section->sec_body_part,
                                  item->att_data.att_static->att_data.att_body_section->sec_length);
                    data = QString::fromStdString(d);
                }

                if (item->att_data.att_static->att_type == MAILIMAP_MSG_ATT_UID) {
                    uid = item->att_data.att_static->att_data.att_uid;
                }
            }
        }

        if (uid == 0) {
            continue;
        }

        if (data.isEmpty()) {
            continue;
        }

        Message *msg = new Message(m_Email, uid);
        msg->setBodyData(data);
        msgList->append(msg);
    }

    mailimap_fetch_list_free(fetchResult);
    mailimap_fetch_type_free(fetchType);
    mailimap_set_free(set);

    return msgList;
}

bool ImapService::checkInternet()
{
    int rv = mailimap_noop(m_Imap);

    return rv == MAILIMAP_NO_ERROR;
}

bool ImapService::deleteMessage(const QString &foldername, QList<ssize_t> uidList)
{
    QMutexLocker locker(&m_Mutex);

    if (!selectFolder(foldername)) {
        return false;
    }

    mailimap_flag_list *flaglist = mailimap_flag_list_new_empty();
    mailimap_flag_list_add(flaglist, mailimap_flag_new_deleted());

    mailimap_set *set = mailimap_set_new_empty();
    for (auto &uid : uidList) {
        mailimap_set_add_single(set, uid);
    }

    mailimap_store_att_flags *storeflags = mailimap_store_att_flags_new_add_flags(flaglist);

    int r = mailimap_uid_store(m_Imap, set, storeflags);

    mailimap_set_free(set);

    if (storeflags != NULL) {
        mailimap_store_att_flags_free(storeflags);
    }

    if (r != MAILIMAP_NO_ERROR) {
        return false;
    }

    r = mailimap_expunge(m_Imap);
    return r == MAILIMAP_NO_ERROR;
}

bool ImapService::moveMessage(const QString &souceFolderName, const QString &destFolderName, QList<ssize_t> uidList)
{
    QMutexLocker locker(&m_Mutex);

    if (!selectFolder(souceFolderName)) {
        return false;
    }

    mailimap_set *set = mailimap_set_new_empty();
    for (auto &uid : uidList) {
        mailimap_set_add_single(set, uid);
    }

    int r = mailimap_uid_move(m_Imap, set, destFolderName.toStdString().c_str());

    mailimap_set_free(set);

    return r == MAILIMAP_NO_ERROR;
}

int ImapService::idleStart(const QString &foldername)
{
    QMutexLocker locker(&m_Mutex);

    if (!selectFolder(foldername)) {
        return -1;
    }

    int rv = mailimap_idle(m_Imap);
    if (rv == MAILIMAP_NO_ERROR) {
        int fd = mailimap_idle_get_fd(m_Imap);
        return fd;
    }

    return -1;
}

bool ImapService::idleDone()
{
    QMutexLocker locker(&m_Mutex);

    int rv = mailimap_idle_done(m_Imap);

    return rv == MAILIMAP_NO_ERROR;
}

QString ImapService::getParentFolderName(QString fullfoldername, QChar delimter)
{
    int lastIndex = fullfoldername.lastIndexOf(delimter);
    if (lastIndex != -1) {
        return fullfoldername.left(lastIndex);
    }

    return "";
}
