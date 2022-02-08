#include "messagelistmodel.h"

#include <QtConcurrent>

MessageListModel::MessageListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_MessageList = new QList<Message *>();

    connect(this, &MessageListModel::messageReadyFinished, this, &MessageListModel::onMessageReadyFinished);
}

int MessageListModel::rowCount(const QModelIndex &parent) const
{
    return m_MessageList->length();
}

QVariant MessageListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    Message *header = m_MessageList->at(index.row());
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
        return flag->SeenFlag();
    case Roles::MessageRecentRole:
        return flag->RecentFlag();
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
    m_MessageList->append(*messageList);
    endInsertRows();
}

void MessageListModel::onFolderSelected(QHash<Account *, Folder *> accountFolder)
{
    qDeleteAll(*m_MessageList);
    m_MessageList->clear();

    setLoading(true);

    for (int i = 0; i < m_Threads.length(); i++) {
        QFuture<void> thread = m_Threads.at(i);
        if (thread.isRunning()) {
            thread.cancel();
        }
    }

    m_Threads.clear();

    QHashIterator<Account *, Folder *> iter(accountFolder);
    while (iter.hasNext()) {
        iter.next();
        Account *account = iter.key();
        Folder *folder = iter.value();

        QtConcurrent::run(this, &MessageListModel::loadMessage, account, folder);
    }
}

void MessageListModel::onMessageReadyFinished(QList<Message *> *msgList)
{
    appendRows(msgList);
}

void MessageListModel::loadMessage(Account *account, Folder *folder)
{
    ImapService *service = account->IMAPService();
    QList<Message *> *msgList = service->getAllHeaders(folder->FullName());

    emit messageReadyFinished(msgList);

    setLoading(false);
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
