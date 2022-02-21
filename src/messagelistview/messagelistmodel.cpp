#include "messagelistmodel.h"

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

    Message *header = m_messageList->at(index.row());
    Flags *flag = header->flags();

    switch (role) {
    case Roles::SenderRole: {
        const Contact *contact = header->From();
        if (contact->Hostname().isEmpty()) {
            return contact->Address();
        }
        return contact->Hostname();
    }
    case Roles::SubjectRole:
        return header->Subject();
    case Roles::DateRole:
        return header->DateTime();
    case Roles::UIDRole:
        return QVariant::fromValue(header->Uid());
    case Roles::EmailRole:
        return header->AccountEmail();
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

        emit messageReadReady(msgList);
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
