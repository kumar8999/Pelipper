#include "imapservice.h"

#include <QDebug>

ImapService::ImapService(const QString &email, const QString &password, const QString &server, int port, ConnectionType connectionType, QObject *parent)
    : m_email(email)
    , m_password(password)
    , m_server(server)
    , m_port(port)
    , m_connectionType(connectionType)
{
    m_imap = mailimap_new(0, NULL);
}

ImapService::~ImapService()
{
    mailimap_free(m_imap);
}

bool ImapService::connect()
{
    QMutexLocker locker(&m_mutex);

    int r = mailimap_ssl_connect(m_imap, m_server.toStdString().c_str(), m_port);

    return (r == MAILIMAP_NO_ERROR_NON_AUTHENTICATED);
}

bool ImapService::login()
{
    QMutexLocker locker(&m_mutex);

    int r = mailimap_login(m_imap, m_email.toStdString().c_str(), m_password.toStdString().c_str());

    return (r == MAILIMAP_NO_ERROR);
}

bool ImapService::logout()
{
    QMutexLocker locker(&m_mutex);

    int r = mailimap_logout(m_imap);

    return r == MAILIMAP_NO_ERROR;
}

bool ImapService::selectFolder(const QString &folderName)
{
    int r = mailimap_select(m_imap, folderName.toStdString().c_str());

    return r == MAILIMAP_NO_ERROR;
}

QList<Folder *> *ImapService::getFolders(QStringList &folderListstr)
{
    QMutexLocker locker(&m_mutex);

    clist *list = NULL;

    int r = mailimap_list(m_imap, "", "*", &list);

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
            const std::string &foldername = std::string(mblist->mb_name);
            folderMap[QString::fromStdString(foldername)] = mblist;
            folderListstr.append(QString::fromStdString(foldername));
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

    return folderList;
}

QList<ssize_t> ImapService::getUids(const QString &foldername)
{
    int r;

    mailimap_set *set = mailimap_set_new_interval(0, 1);
    clist *fetchResult = NULL;
    mailimap_fetch_type *fetchType = mailimap_fetch_type_new_fetch_att_list_empty();
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, mailimap_fetch_att_new_uid());

    r = mailimap_uid_fetch(m_imap, set, fetchType, &fetchResult);

    if (r != MAILIMAP_NO_ERROR) {
        qDebug() << "get uids" << r;
        mailimap_fetch_type_free(fetchType);
        mailimap_set_free(set);
        return QList<ssize_t>();
    }

    //    }
    QList<ssize_t> uidList;

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

    mailimap_fetch_list_free(fetchResult);
    mailimap_fetch_type_free(fetchType);
    mailimap_set_free(set);

    return uidList;
}

QList<Message *> *ImapService::getAllHeaders(const QString &foldername, const QList<ssize_t> &uidList)
{
    QMutexLocker locker(&m_mutex);

    if (!selectFolder(foldername)) {
        return nullptr;
    }

    QList<Message *> *msgList;
    mailimap_set *set = mailimap_set_new_empty();
    clist *fetchResult = NULL;
    mailimap_fetch_type *fetchType = mailimap_fetch_type_new_fetch_att_list_empty();
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, mailimap_fetch_att_new_rfc822_header());
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, mailimap_fetch_att_new_uid());
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, mailimap_fetch_att_new_flags());

    if (!uidList.isEmpty()) {
        for (auto uid : qAsConst(uidList)) {
            mailimap_set_add_single(set, uid);
        }
    }

    int r = mailimap_uid_fetch(m_imap, set, fetchType, &fetchResult);

    if (r != MAILIMAP_NO_ERROR) {
        qDebug() << "error headers code: " << r;
        mailimap_fetch_type_free(fetchType);
        mailimap_set_free(set);
        return nullptr;
    }

    msgList = new QList<Message *>();

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
                        parseFlags(flagFetch, flags);
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
            delete flags;
            continue;
        }

        Message *msg = new Message(m_email, uid);
        msg->parseFromData(data);
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
    QMutexLocker locker(&m_mutex);

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

    r = mailimap_uid_fetch(m_imap, set, fetchType, &fetchResult);

    if (r != MAILIMAP_NO_ERROR) {
        qDebug() << "body code: " << r;
        mailimap_fetch_type_free(fetchType);
        mailimap_set_free(set);
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

                        parseFlags(flagFetch, flags);
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
    }

    Message *msg = new Message(m_email, uid);
    msg->setFlags(flags);
    msg->parseFromData(data);

    mailimap_fetch_list_free(fetchResult);
    mailimap_fetch_type_free(fetchType);
    mailimap_set_free(set);

    return msg;
}

QList<Message *> *ImapService::getAllMessage(const QString &foldername)
{
    mailimap_set *set;
    mailimap_section *section;
    mailimap_fetch_type *fetchType;
    mailimap_fetch_att *fetchAtt;
    clist *fetchResult = NULL;
    QList<Message *> *msgList;

    if (!selectFolder(foldername)) {
        return nullptr;
    }

    section = mailimap_section_new(NULL);
    set = mailimap_set_new_interval(0, 1);
    fetchType = mailimap_fetch_type_new_fetch_att_list_empty();
    fetchAtt = mailimap_fetch_att_new_body_peek_section(section);
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, mailimap_fetch_att_new_flags());
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, mailimap_fetch_att_new_uid());
    mailimap_fetch_type_new_fetch_att_list_add(fetchType, fetchAtt);

    int r = mailimap_uid_fetch(m_imap, set, fetchType, &fetchResult);

    if (r != MAILIMAP_NO_ERROR) {
        qDebug() << "body code: " << r;
        mailimap_fetch_type_free(fetchType);
        mailimap_set_free(set);
        return nullptr;
    }

    msgList = new QList<Message *>();

    for (clistiter *it = clist_begin(fetchResult); it != NULL; it = clist_next(it)) {
        mailimap_msg_att *msg_att = (mailimap_msg_att *)clist_content(it);

        QString data;
        ssize_t uid = 0;
        Flags *flags = new Flags();

        for (clistiter *cur = clist_begin(msg_att->att_list); cur != NULL; cur = clist_next(cur)) {
            mailimap_msg_att_item *item = (mailimap_msg_att_item *)clist_content(cur);

            if (item->att_type == MAILIMAP_MSG_ATT_ITEM_DYNAMIC) {
                if (item->att_data.att_dyn->att_list != NULL) {
                    for (clistiter *dit = clist_begin(item->att_data.att_dyn->att_list); dit != NULL; dit = clist_next(dit)) {
                        mailimap_flag_fetch *flagFetch = (mailimap_flag_fetch *)clist_content(dit);
                        parseFlags(flagFetch, flags);
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
            delete flags;
            continue;
        }

        Message *msg = new Message(m_email, uid);
        msg->setFlags(flags);
        msg->parseFromData(data);
        msgList->append(msg);
    }

    mailimap_fetch_list_free(fetchResult);
    mailimap_fetch_type_free(fetchType);
    mailimap_set_free(set);

    return msgList;
}

QList<Message *> *ImapService::fetchMessage(const QString &foldername, const QList<ssize_t> &uidList, FetchType fetchType)
{
    mailimap_set *set;
    mailimap_section *section;
    mailimap_fetch_type *mailimapfetchType;
    mailimap_fetch_att *fetchAtt;
    clist *fetchResult = NULL;
    QList<Message *> *msgList;

    if (!selectFolder(foldername)) {
        return nullptr;
    }

    section = mailimap_section_new(NULL);
    set = mailimap_set_new_empty();
    mailimap_fetch_type_new_fetch_att_list_add(mailimapfetchType, mailimap_fetch_att_new_uid());

    if (fetchType & FetchType::Body) {
        mailimapfetchType = mailimap_fetch_type_new_fetch_att_list_empty();
        fetchAtt = mailimap_fetch_att_new_body_peek_section(section);
        mailimap_fetch_type_new_fetch_att_list_add(mailimapfetchType, fetchAtt);
    } else if (fetchType & FetchType::BodyWithNoSeen) {
        mailimapfetchType = mailimap_fetch_type_new_fetch_att_list_empty();
        fetchAtt = mailimap_fetch_att_new_body_peek_section(section);
        mailimap_fetch_type_new_fetch_att_list_add(mailimapfetchType, fetchAtt);
    } else if (fetchType & FetchType::Header) {
        mailimap_fetch_type_new_fetch_att_list_add(mailimapfetchType, mailimap_fetch_att_new_rfc822_header());
        mailimap_fetch_type_new_fetch_att_list_add(mailimapfetchType, fetchAtt);
    } else if (fetchType & FetchType::Flag) {
        mailimapfetchType = mailimap_fetch_type_new_fetch_att_list_empty();
        mailimap_fetch_type_new_fetch_att_list_add(mailimapfetchType, mailimap_fetch_att_new_flags());
    }

    int r = mailimap_uid_fetch(m_imap, set, mailimapfetchType, &fetchResult);

    if (r != MAILIMAP_NO_ERROR) {
        qDebug() << "body code: " << r;
        mailimap_fetch_type_free(mailimapfetchType);
        mailimap_set_free(set);
        return nullptr;
    }

    msgList = new QList<Message *>();

    for (clistiter *it = clist_begin(fetchResult); it != NULL; it = clist_next(it)) {
        mailimap_msg_att *msg_att = (mailimap_msg_att *)clist_content(it);

        QString data;
        ssize_t uid = 0;
        Flags *flags = new Flags();

        for (clistiter *cur = clist_begin(msg_att->att_list); cur != NULL; cur = clist_next(cur)) {
            mailimap_msg_att_item *item = (mailimap_msg_att_item *)clist_content(cur);

            if (item->att_type == MAILIMAP_MSG_ATT_ITEM_DYNAMIC) {
                if (item->att_data.att_dyn->att_list != NULL) {
                    for (clistiter *dit = clist_begin(item->att_data.att_dyn->att_list); dit != NULL; dit = clist_next(dit)) {
                        mailimap_flag_fetch *flagFetch = (mailimap_flag_fetch *)clist_content(dit);
                        parseFlags(flagFetch, flags);
                    }
                }
            }

            if (item->att_type == MAILIMAP_MSG_ATT_ITEM_STATIC) {
                if (item->att_data.att_static->att_type == MAILIMAP_MSG_ATT_BODY_SECTION) {
                    std::string d(item->att_data.att_static->att_data.att_body_section->sec_body_part,
                                  item->att_data.att_static->att_data.att_body_section->sec_length);
                    data = QString::fromStdString(d);
                }

                if (item->att_data.att_static->att_type == MAILIMAP_MSG_ATT_BODY_SECTION) {
                    std::string d(item->att_data.att_static->att_data.att_body_section->sec_body_part,
                                  item->att_data.att_static->att_data.att_body_section->sec_length);
                    data = QString::fromStdString(d);
                }

                if (item->att_data.att_static->att_type == MAILIMAP_MSG_ATT_UID) {
                    uid = item->att_data.att_static->att_data.att_uid;
                }

                if (item->att_data.att_static->att_type == MAILIMAP_MSG_ATT_RFC822_HEADER) {
                    std::string hData = std::string(item->att_data.att_static->att_data.att_rfc822_header.att_content,
                                                    item->att_data.att_static->att_data.att_rfc822_header.att_length);
                    data = QString::fromStdString(hData);
                }
            }
        }

        if (uid == 0) {
            delete flags;
            continue;
        }

        Message *msg = new Message(m_email, uid);
        msg->setFlags(flags);
        msg->parseFromData(data);
        msgList->append(msg);
    }

    mailimap_fetch_list_free(fetchResult);
    mailimap_fetch_type_free(mailimapfetchType);
    mailimap_set_free(set);

    return msgList;
}

bool ImapService::checkInternet()
{
    int rv = mailimap_noop(m_imap);

    return rv == MAILIMAP_NO_ERROR;
}

bool ImapService::deleteMessage(const QString &foldername, QList<ssize_t> uidList)
{
    QMutexLocker locker(&m_mutex);

    if (uidList.isEmpty())
        return false;

    if (!selectFolder(foldername)) {
        qDebug() << "could not select";
        return false;
    }

    mailimap_flag_list *flaglist = mailimap_flag_list_new_empty();
    mailimap_flag_list_add(flaglist, mailimap_flag_new_deleted());
    mailimap_store_att_flags *storeflags = mailimap_store_att_flags_new_add_flags(flaglist);

    mailimap_set *set = mailimap_set_new_empty();
    for (auto &uid : uidList) {
        mailimap_set_add_single(set, uid);
    }

    int r = mailimap_uid_store(m_imap, set, storeflags);
    mailimap_set_free(set);
    mailimap_store_att_flags_free(storeflags);

    if (r != MAILIMAP_NO_ERROR) {
        qDebug() << "could not delete" << r;
        return false;
    }

    //    r = mailimap_expunge(m_imap);
    return r == MAILIMAP_NO_ERROR;
}

bool ImapService::moveMessage(const QString &souceFolderName, const QString &destFolderName, QList<ssize_t> uidList)
{
    QMutexLocker locker(&m_mutex);

    if (uidList.isEmpty()) {
        return false;
    }

    if (!selectFolder(souceFolderName)) {
        return false;
    }

    mailimap_set *set = mailimap_set_new_empty();
    for (auto &uid : uidList) {
        mailimap_set_add_single(set, uid);
    }

    int r = mailimap_uid_move(m_imap, set, destFolderName.toStdString().c_str());

    mailimap_set_free(set);

    return r == MAILIMAP_NO_ERROR;
}

int ImapService::idleStart(const QString &foldername)
{
    QMutexLocker locker(&m_mutex);

    if (!selectFolder(foldername)) {
        qDebug() << "select ";
        return -1;
    }

    int rv = mailimap_idle(m_imap);

    qDebug() << "idle" << rv;
    if (rv == MAILIMAP_NO_ERROR) {
        int fd = mailimap_idle_get_fd(m_imap);

        qDebug() << "idle get fd" << fd;

        return fd;
    }

    return -1;
}

bool ImapService::idleDone()
{
    QMutexLocker locker(&m_mutex);

    int rv = mailimap_idle_done(m_imap);

    return rv == MAILIMAP_NO_ERROR;
}

QString ImapService::getParentFolderName(const QString &fullfoldername, const QChar &delimter)
{
    int lastIndex = fullfoldername.lastIndexOf(delimter);
    if (lastIndex != -1) {
        return fullfoldername.left(lastIndex);
    }

    return "";
}

void ImapService::parseFlags(mailimap_flag_fetch *flagFetch, Flags *flags)
{
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
