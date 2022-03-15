#include "messagelistmodel.h"

#include "../backend/session.h"

#include <QtConcurrent>

MessageListModel::MessageListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_messageList = new QList<Message *>();

    connect(this, &MessageListModel::messageReadReady, this, &MessageListModel::onMessageReadyFinished);
}

MessageListModel::~MessageListModel()
{
    qDeleteAll(*m_messageList);
    delete m_messageList;
}

int MessageListModel::rowCount(const QModelIndex &parent) const
{
    return m_messageList->length();
}

QVariant MessageListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    Message *message = m_messageList->at(index.row());
    MessageHeader *header = m_messageList->at(index.row())->header();
    Flags *flag = message->flags();

    if (header == nullptr) {
        return QVariant();
    }

    switch (role) {
    case Roles::SenderRole: {
        const Address *address = header->from();
        if (address->displayName().isEmpty()) {
            return address->address();
        }
        return address->displayName();
    }
    case Roles::SubjectRole:
        return header->subject();
    case Roles::DateRole:
        return QVariant(header->dateTime());
    case Roles::UIDRole:
        return QVariant::fromValue(message->uid());
    case Roles::EmailRole:
        return message->accountEmail();
    case Roles::MessageSeenRole:
        return flag->seenFlag();
    case Roles::MessageRecentRole:
        return flag->recentFlag();
    }

    return QVariant();
}

QHash<int, QByteArray> MessageListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Roles::SubjectRole] = "Subject";
    roles[Roles::SenderRole] = "Sender";
    roles[Roles::DateRole] = "Date";
    roles[Roles::UIDRole] = "Uid";
    roles[Roles::EmailRole] = "Email";
    roles[Roles::MessageSeenRole] = "Seen";
    roles[Roles::MessageRecentRole] = "Recent";
    roles[Roles::SelectedRole] = "Selected";
    return roles;
}

void MessageListModel::appendRows(QList<Message *> *messageList)
{
    if (messageList->isEmpty()) {
        return;
    }

    beginInsertRows(QModelIndex(), rowCount(QModelIndex()), messageList->count() - 1 + rowCount(QModelIndex()));
    m_messageList->append(*messageList);
    endInsertRows();
}

void MessageListModel::onFolderSelected(QHash<Account *, Folder *> *accountFolder)
{
    beginResetModel();
    qDeleteAll(*m_messageList);
    m_messageList->clear();
    endResetModel();

    if (m_future.isRunning()) {
        m_future.cancel();
    }

    m_future = QtConcurrent::run(this, &MessageListModel::loadMessage, accountFolder);
}

void MessageListModel::setSeenFlag(int row)
{
    Message *message = m_messageList->at(row);

    Flags *flag = message->flags();
    flag->setSeenFlag(true);
}

void MessageListModel::onMessageReadyFinished(QList<Message *> *msgList)
{
    setLoading(false);
    appendRows(msgList);
}

QString MessageListModel::parseDate(const QDateTime &datetime) const
{
    QDateTime currentDate = QDateTime::currentDateTime();

    if (datetime.daysTo(currentDate) < 1) {
        int seconds = datetime.secsTo(currentDate);
        const qint64 DAY = 86400;
        QTime t = QTime(0, 0).addSecs(seconds % DAY);

        if (t.hour() > 1) {
            return QString::number(t.hour()) + " ago";
        } else {
            return "less than 1hr ago";
        }

    } else if (datetime.daysTo(currentDate) < 2) {
        return "Yesterday";
    } else if (datetime.daysTo(currentDate) < 7) {
        return datetime.date().toString("ddd");
    } else {
        return datetime.date().toString("MMM d");
    }
}

void MessageListModel::loadMessage(QHash<Account *, Folder *> *accountFolder)
{
    setLoading(true);

    QHashIterator<Account *, Folder *> iter(*accountFolder);

    while (iter.hasNext()) {
        iter.next();
        Account *account = iter.key();
        Folder *folder = iter.value();

        ImapService *service = account->IMAPService();
        QList<Message *> *msgList = service->getAllHeaders(folder->FullName());

        if (msgList == nullptr) {
            qDebug() << "error loading headers";
            continue;
        }

        for (int i = 0; i < msgList->length(); i++) {
            Message *msg = msgList->at(i);
            folders.insert(msg->uid(), folder->FullName());
        }

        emit messageReadReady(msgList);
    }
}

void MessageListModel::_deleteMessages(QModelIndexList indexList)
{
    auto *deleteUids = new QMap<QString, QList<ssize_t>>();

    for (auto index : indexList) {
        Message *message = m_messageList->at(index.row());
        ssize_t uid = message->uid();
        QString accountEmail = message->accountEmail();
        QString folderName = folders[uid];

        if (deleteUids->contains(folderName + " - " + accountEmail)) {
            deleteUids->insert(folderName + " - " + accountEmail, QList<ssize_t>());
        }

        QList<ssize_t> uidList = deleteUids->value(folderName);
        uidList.append(uid);
        deleteUids->insert(folderName + " - " + accountEmail, uidList);
    }

    QMapIterator<QString, QList<ssize_t>> deleteUidIter(*deleteUids);
    while (deleteUidIter.hasNext()) {
        deleteUidIter.next();
        qDebug() << deleteUidIter.key();
        QList<QString> splits = deleteUidIter.key().split(" - ");

        QString folderName = splits.at(0);
        QString accountEmail = splits.at(1);

        qDebug() << accountEmail << "delete" << folderName;
        QList<ssize_t> uidList = deleteUids->value(folderName);

        Account *account = Session::getInstance()->getAccount(accountEmail);
        ImapService *imapService = account->IMAPService();
        imapService->deleteMessage(folderName, uidList);
    }
}

void MessageListModel::_moveMessages(QModelIndexList indexList)
{
    auto *moveUids = new QMap<QString, QList<ssize_t>>();

    for (auto index : indexList) {
        Message *message = m_messageList->at(index.row());
        ssize_t uid = message->uid();
        QString accountEmail = message->accountEmail();
        QString folderName = folders[uid];

        if (moveUids->contains(folderName + " - " + accountEmail)) {
            moveUids->insert(folderName + " - " + accountEmail, QList<ssize_t>());
        }

        QList<ssize_t> uidList = moveUids->value(folderName);
        uidList.append(uid);
        moveUids->insert(folderName + " - " + accountEmail, uidList);
    }

    QMapIterator<QString, QList<ssize_t>> moveUidIter(*moveUids);
    while (moveUidIter.hasNext()) {
        moveUidIter.next();
        qDebug() << moveUidIter.key();
        QList<QString> splits = moveUidIter.key().split(" - ");

        QString folderName = splits.at(0);
        QString accountEmail = splits.at(1);

        qDebug() << accountEmail << "delete" << folderName;
        QList<ssize_t> uidList = moveUids->value(folderName);

        Account *account = Session::getInstance()->getAccount(accountEmail);
        ImapService *imapService = account->IMAPService();
        //        imapService->moveMessage(folderName, uidList);
    }
}

bool MessageListModel::loading() const
{
    return m_loading;
}

void MessageListModel::setLoading(bool newLoading)
{
    if (m_loading == newLoading)
        return;
    m_loading = newLoading;
    emit loadingChanged();
}

void MessageListModel::deleteMessages(QModelIndexList indexList)
{
    QtConcurrent::run(this, &MessageListModel::_deleteMessages, indexList);
}

void MessageListModel::moveMessages(QModelIndexList indexList)
{
    QtConcurrent::run(this, &MessageListModel::_moveMessages, indexList);
}
