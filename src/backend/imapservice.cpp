#include "imapservice.h"

#include <QDebug>

ImapService::ImapService(const QString &email, const QString &password, const QString &server, int port)
    : m_Email(email)
    , m_Password(password)
    , m_Server(server)
    , m_Port(port)
{
    m_Imap = mailimap_new(0, NULL);
}

ImapService::~ImapService()
{
    mailimap_free(m_Imap);
}

bool ImapService::connect()
{
    QMutexLocker locker(&m_Mutex);

    int r = mailimap_ssl_connect(m_Imap, m_Server.toStdString().c_str(), m_Port);

    qDebug() << m_Email << "connect" << r;

    return r == MAILIMAP_NO_ERROR_NON_AUTHENTICATED;
}

bool ImapService::login()
{
    QMutexLocker locker(&m_Mutex);

    int r = mailimap_login(m_Imap, m_Email.toStdString().c_str(), m_Password.toStdString().c_str());

    qDebug() << m_Email << "login" << r;

    return r == MAILIMAP_NO_ERROR;
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

    parseFolders(folderMap, folderList);

    mailimap_list_result_free(list);

    return folderList;
}

bool ImapService::getUids(const QString &foldername, QList<ssize_t> &uidList)
{
    QMutexLocker locker(&m_Mutex);

    mailimap_set *set;
    mailimap_fetch_type *fetchType;
    clist *fetchResult;
    int r;

    set = mailimap_set_new_interval(1, 0);
    fetchType = mailimap_fetch_type_new_fetch_att_list_empty();
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, mailimap_fetch_att_new_uid());

    r = mailimap_uid_fetch(m_Imap, set, fetchType, &fetchResult);

    if (r != MAILIMAP_NO_ERROR) {
        qDebug() << "get uids";
        qDebug() << "Errorcode: " << r;
        return false;
    }

    for (clistiter *it = clist_begin(fetchResult); it != NULL; it = clist_next(it)) {
        struct mailimap_msg_att *msgAtt = (struct mailimap_msg_att *)clist_content(it);

        uint32_t uid = 0;

        for (clistiter *cur = clist_begin(msgAtt->att_list); cur != NULL; cur = clist_next(cur)) {
            struct mailimap_msg_att_item *item = (struct mailimap_msg_att_item *)clist_content(cur);

            if (item->att_type == MAILIMAP_MSG_ATT_ITEM_DYNAMIC) {
                continue;
            }

            if (item->att_type == MAILIMAP_MSG_ATT_ITEM_STATIC) {
                if (item->att_data.att_static->att_type == MAILIMAP_MSG_ATT_UID) {
                    uid = item->att_data.att_static->att_data.att_uid;
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

    mailimap_fetch_list_free(fetchResult);
    mailimap_fetch_type_free(fetchType);
    mailimap_set_free(set);

    return true;
}

QList<Message *> *ImapService::getAllHeaders(const QString &foldername, QList<ssize_t> headerUidList)
{
    QMutexLocker locker(&m_Mutex);

    struct mailimap_set *set = mailimap_set_new_interval(0, 1);
    clist *fetchResult = NULL;
    struct mailimap_fetch_type *fetchType = mailimap_fetch_type_new_fetch_att_list_empty();
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, mailimap_fetch_att_new_rfc822_header());
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, mailimap_fetch_att_new_uid());

    if (!headerUidList.isEmpty()) {
        set = mailimap_set_new_empty();
        for (auto uid : qAsConst(headerUidList)) {
            mailimap_set_add_single(set, uid);
        }
    }

    if (!selectFolder(foldername)) {
        return nullptr;
    }

    int r = mailimap_uid_fetch(m_Imap, set, fetchType, &fetchResult);

    if (r != MAILIMAP_NO_ERROR) {
        qDebug() << "headers code: " << r;
        return nullptr;
    }

    QList<Message *> *msgList = new QList<Message *>();

    for (clistiter *it = clist_begin(fetchResult); it != NULL; it = clist_next(it)) {
        mailimap_msg_att *msgAtt = (mailimap_msg_att *)clist_content(it);

        QString data;
        ssize_t uid = 0;

        for (clistiter *cur = clist_begin(msgAtt->att_list); cur != NULL; cur = clist_next(cur)) {
            mailimap_msg_att_item *item = (mailimap_msg_att_item *)clist_content(cur);

            if (item->att_type == MAILIMAP_MSG_ATT_ITEM_DYNAMIC)
                continue;

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

    struct mailimap_set *set;
    struct mailimap_section *section;
    struct mailimap_fetch_type *fetchType;
    mailimap_fetch_att *fetchAtt;
    clist *fetchResult = NULL;
    int r;

    section = mailimap_section_new(NULL);
    set = mailimap_set_new_single(uid);
    fetchType = mailimap_fetch_type_new_fetch_att_list_empty();
    fetchAtt = mailimap_fetch_att_new_body_section(section);
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, mailimap_fetch_att_new_uid());
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, fetchAtt);

    r = mailimap_uid_fetch(m_Imap, set, fetchType, &fetchResult);

    if (r != MAILIMAP_NO_ERROR) {
        qDebug() << "body code: " << r;
        return nullptr;
    }
    QString data;

    for (clistiter *it = clist_begin(fetchResult); it != NULL; it = clist_next(it)) {
        struct mailimap_msg_att *msg_att = (struct mailimap_msg_att *)clist_content(it);

        for (clistiter *cur = clist_begin(msg_att->att_list); cur != NULL; cur = clist_next(cur)) {
            struct mailimap_msg_att_item *item = (struct mailimap_msg_att_item *)clist_content(cur);

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
    }

    Message *msg = new Message(uid);
    msg->setBodyData(data);

    mailimap_fetch_list_free(fetchResult);
    mailimap_fetch_type_free(fetchType);
    mailimap_set_free(set);

    return msg;
}

bool ImapService::getAllBody(QMap<ssize_t, QString> &bodyDatas, QList<ssize_t> nonCachedUidList)
{
    QMutexLocker locker(&m_Mutex);

    struct mailimap_set *set;
    struct mailimap_section *section;
    struct mailimap_fetch_type *fetchType;
    mailimap_fetch_att *fetchAtt;
    clist *fetchResult = NULL;

    section = mailimap_section_new(NULL);
    set = mailimap_set_new_interval(0, 1);
    fetchType = mailimap_fetch_type_new_fetch_att_list_empty();
    fetchAtt = mailimap_fetch_att_new_body_section(section);
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, mailimap_fetch_att_new_uid());
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, fetchAtt);

    if (!nonCachedUidList.isEmpty()) {
        set = mailimap_set_new_empty();
    }

    for (auto uid : qAsConst(nonCachedUidList)) {
        mailimap_set_add_single(set, uid);
    }

    int r = mailimap_uid_fetch(m_Imap, set, fetchType, &fetchResult);

    if (r != MAILIMAP_NO_ERROR) {
        qDebug() << "body code: " << r;
        return false;
    }

    for (clistiter *it = clist_begin(fetchResult); it != NULL; it = clist_next(it)) {
        struct mailimap_msg_att *msg_att = (struct mailimap_msg_att *)clist_content(it);

        QString data;
        ssize_t uid = 0;

        for (clistiter *cur = clist_begin(msg_att->att_list); cur != NULL; cur = clist_next(cur)) {
            struct mailimap_msg_att_item *item = (struct mailimap_msg_att_item *)clist_content(cur);

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

        bodyDatas[uid] = data;
    }

    mailimap_fetch_list_free(fetchResult);
    mailimap_fetch_type_free(fetchType);
    mailimap_set_free(set);

    return true;
}

bool ImapService::getFlags(const QString &foldername, QList<ssize_t> uidList, QMap<ssize_t, QList<bool>> &flagsList)
{
    if (uidList.empty()) {
        return true;
    }

    struct mailimap_set *set = mailimap_set_new_empty();
    for (auto &uid : uidList) {
        mailimap_set_add_single(set, uid);
    }

    QMutexLocker locker(&m_Mutex);

    if (!selectFolder(foldername)) {
        mailimap_set_free(set);
        return false;
    }

    struct mailimap_fetch_type *fetch_type = mailimap_fetch_type_new_fetch_att_list_empty();
    mailimap_fetch_type_new_fetch_att_list_add(fetch_type, mailimap_fetch_att_new_uid());
    mailimap_fetch_type_new_fetch_att_list_add(fetch_type, mailimap_fetch_att_new_flags());

    clist *fetch_result = NULL;

    int rv = mailimap_uid_fetch(m_Imap, set, fetch_type, &fetch_result);

    if (rv != MAILIMAP_NO_ERROR) {
        return false;
    }

    for (clistiter *it = clist_begin(fetch_result); it != NULL; it = clist_next(it)) {
        struct mailimap_msg_att *msg_att = (struct mailimap_msg_att *)clist_content(it);

        uint32_t uid = 0;
        QList<bool> flags;

        for (int i = 0; i < 3; i++) {
            flags.insert(i, false);
        }

        for (clistiter *ait = clist_begin(msg_att->att_list); ait != NULL; ait = clist_next(ait)) {
            struct mailimap_msg_att_item *item = (struct mailimap_msg_att_item *)clist_content(ait);

            if (item->att_type == MAILIMAP_MSG_ATT_ITEM_DYNAMIC) {
                if (item->att_data.att_dyn->att_list != NULL) {
                    for (clistiter *dit = clist_begin(item->att_data.att_dyn->att_list); dit != NULL; dit = clist_next(dit)) {
                        struct mailimap_flag_fetch *flagFetch = (struct mailimap_flag_fetch *)clist_content(dit);
                        if (flagFetch && flagFetch->fl_flag) {
                            switch (flagFetch->fl_flag->fl_type) {
                            case MAILIMAP_FLAG_SEEN:
                                flags.insert(0, true);
                                break;

                            case MAILIMAP_FLAG_DELETED:
                                flags.insert(1, true);
                                break;

                            case MAILIMAP_FLAG_ANSWERED:
                                flags.insert(2, true);
                                break;

                            case MAILIMAP_FLAG_FLAGGED:
                                break;

                            case MAILIMAP_FLAG_DRAFT:
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
            } else if (item->att_type == MAILIMAP_MSG_ATT_ITEM_STATIC) {
                if (item->att_data.att_static->att_type == MAILIMAP_MSG_ATT_UID) {
                    uid = item->att_data.att_static->att_data.att_uid;
                }
            }
        }

        if (uid == 0) {
            continue;
        }

        flagsList[uid] = flags;
    }

    mailimap_fetch_list_free(fetch_result);

    mailimap_fetch_type_free(fetch_type);
    mailimap_set_free(set);

    return (rv == MAILIMAP_NO_ERROR);
}

bool ImapService::checkInternet()
{
    QMutexLocker locker(&m_Mutex);

    int rv = mailimap_noop(m_Imap);

    return rv == MAILIMAP_NO_ERROR;
}

bool ImapService::deleteMessage(const QString &foldername, QList<ssize_t> uidList)
{
    QMutexLocker locker(&m_Mutex);

    if (!selectFolder(foldername)) {
        return false;
    }

    struct mailimap_flag_list *flaglist = mailimap_flag_list_new_empty();
    mailimap_flag_list_add(flaglist, mailimap_flag_new_deleted());

    struct mailimap_set *set = mailimap_set_new_empty();
    for (auto &uid : uidList) {
        mailimap_set_add_single(set, uid);
    }

    struct mailimap_store_att_flags *storeflags = mailimap_store_att_flags_new_add_flags(flaglist);

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

    struct mailimap_set *set = mailimap_set_new_empty();
    for (auto &uid : uidList) {
        mailimap_set_add_single(set, uid);
    }

    int r = mailimap_uid_move(m_Imap, set, destFolderName.toStdString().c_str());

    mailimap_set_free(set);

    return r == MAILIMAP_NO_ERROR;
}

void ImapService::parseFolders(QMap<QString, mailimap_mailbox_list *> folderMap, QList<Folder *> *&folderList)
{
    m_Folders.clear();
    QMapIterator<QString, mailimap_mailbox_list *> folderMapIter(folderMap);

    while (folderMapIter.hasNext()) {
        folderMapIter.next();

        parseFolder(folderMapIter.key().split("/"), 0, folderMapIter.value(), folderList);
    }
}

void ImapService::parseFolder(QStringList foldernameList, int index, mailimap_mailbox_list *mb, QList<Folder *> *&folderList)
{
    QString folderName = getFolderName(foldernameList, index);

    Folder *item = m_Folders[folderName];

    if (item == nullptr) {
        QString parentFolderName = getParentFolderName(foldernameList, index);
        Folder *parentItem = m_Folders[parentFolderName];

        item = new Folder(folderName, QChar(mb->mb_delimiter));

        mailimap_mbx_list_flags *bflags = mb->mb_flag;

        for (clistiter *iter = clist_begin(bflags->mbf_oflags); iter != NULL; iter = iter->next) {
            mailimap_mbx_list_oflag *flg = (mailimap_mbx_list_oflag *)clist_content(iter);

            if (flg->of_type == MAILIMAP_MBX_LIST_OFLAG_FLAG_EXT) {
                const std::string flgExt = std::string(flg->of_flag_ext);
                if (flgExt == "HasChildren") {
                } else if (flgExt == "HasNoChildren") {
                }
            }
        }

        if (parentItem == nullptr) {
            folderList->append(item);
        } else {
            parentItem->appendChild(item);
        }
        m_Folders[folderName] = item;
        if (foldernameList.length() - 1 != index) {
            index = index + 1;
            parseFolder(foldernameList, index, mb, folderList);
        }
    } else {
        if (foldernameList.length() - 1 != index) {
            index = index + 1;
            parseFolder(foldernameList, index, mb, folderList);
        }
    }
}

QString ImapService::getFolderName(QStringList folderList, int index)
{
    QString foldername = "";
    for (int i = 0; i <= index; i++) {
        foldername += folderList.at(i) + "/";
    }
    int lastSeperatorPos = foldername.lastIndexOf("/");
    return foldername.left(lastSeperatorPos);
}

QString ImapService::getParentFolderName(QStringList folderList, int index)
{
    QString foldername = "";
    for (int i = 0; i < index; i++) {
        foldername += folderList.at(i) + "/";
    }
    int lastSeperatorPos = foldername.lastIndexOf("/");
    return foldername.left(lastSeperatorPos);
}
