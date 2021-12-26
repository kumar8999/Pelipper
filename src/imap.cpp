#include "imap.h"

#include <QDebug>

Imap::Imap(const QString &email, const QString &password, const QString &server, int port)
    : m_Email(email)
    , m_Password(password)
    , m_Server(server)
    , m_Port(port)
{
    m_Imap = mailimap_new(0, NULL);
}

Imap::~Imap()
{
    mailimap_free(m_Imap);
}

bool Imap::connect()
{
    QMutexLocker locker(&m_Mutex);

    int r = mailimap_ssl_connect(m_Imap, m_Server.toStdString().c_str(), m_Port);

    return r == MAILIMAP_NO_ERROR_NON_AUTHENTICATED;
}

bool Imap::login()
{
    QMutexLocker locker(&m_Mutex);

    int r = mailimap_login(m_Imap, m_Email.toStdString().c_str(), m_Password.toStdString().c_str());

    return r == MAILIMAP_NO_ERROR;
}

bool Imap::logout()
{
    QMutexLocker locker(&m_Mutex);

    int r = mailimap_logout(m_Imap);

    return r == MAILIMAP_NO_ERROR;
}

bool Imap::selectFolder(QString folderName)
{
    int r = mailimap_select(m_Imap, folderName.toStdString().c_str());
    m_SelectedFolder = folderName;

    return r == MAILIMAP_NO_ERROR;
}

bool Imap::getFolders(QString folderName, QStringList &folderList)
{
    QMutexLocker locker(&m_Mutex);

    clist *list = NULL;

    int r = mailimap_list(m_Imap, "", folderName.toStdString().c_str(), &list);

    if (r != MAILIMAP_NO_ERROR) {
        qDebug() << "Could not fetch folder list";
        qDebug() << "Errorcode: " << r;
        return false;
    }

    for (clistiter *it = clist_begin(list); it != NULL; it = it->next) {
        struct mailimap_mailbox_list *mblist = (struct mailimap_mailbox_list *)clist_content(it);
        if (mblist) {
            mailimap_mbx_list_flags *bflags = mblist->mb_flag;
            if (bflags) {
                if (!((bflags->mbf_type == MAILIMAP_MBX_LIST_FLAGS_SFLAG) && (bflags->mbf_sflag == MAILIMAP_MBX_LIST_SFLAG_NOSELECT))) {
                    const std::string &foldername = std::string(mblist->mb_name);
                    folderList << QString::fromStdString(foldername);
                }
            }
        }
    }

    mailimap_list_result_free(list);
    return true;
}

bool Imap::getUids(const QString &foldername, QList<ssize_t> &uidList)
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

bool Imap::getAllHeaders(const QString &foldername, QList<ssize_t> headerUidList, QMap<ssize_t, QString> &headerDatas)
{
    QMutexLocker locker(&m_Mutex);

    struct mailimap_set *set = mailimap_set_new_interval(0, 1);
    clist *fetchResult = NULL;
    struct mailimap_fetch_type *fetchType = mailimap_fetch_type_new_fetch_att_list_empty();
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, mailimap_fetch_att_new_rfc822_header());
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, mailimap_fetch_att_new_uid());

    if (!headerUidList.isEmpty()) {
        set = mailimap_set_new_empty();
    }

    for (auto uid : qAsConst(headerUidList)) {
        mailimap_set_add_single(set, uid);
    }

    int r = mailimap_uid_fetch(m_Imap, set, fetchType, &fetchResult);

    if (r != MAILIMAP_NO_ERROR) {
        qDebug() << "headers code: " << r;
        return false;
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

        headerDatas[uid] = data;
    }

    mailimap_fetch_list_free(fetchResult);
    mailimap_fetch_type_free(fetchType);
    mailimap_set_free(set);

    return true;
}

bool Imap::getBody(ssize_t uid, QString &data)
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
        return false;
    }

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

    mailimap_fetch_list_free(fetchResult);
    mailimap_fetch_type_free(fetchType);
    mailimap_set_free(set);

    return true;
}

bool Imap::getAllBody(QMap<ssize_t, QString> &bodyDatas, QList<ssize_t> nonCachedUidList)
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

bool Imap::checkInternet()
{
    QMutexLocker locker(&m_Mutex);

    int rv = mailimap_noop(m_Imap);

    return rv == MAILIMAP_NO_ERROR;
}

bool Imap::deleteMessage(const QString &foldername, QList<ssize_t> uidList)
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

bool Imap::moveMessage(const QString &souceFolderName, const QString &destFolderName, QList<ssize_t> uidList)
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
