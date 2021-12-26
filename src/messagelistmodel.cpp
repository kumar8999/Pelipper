#include "messagelistmodel.h"

#include "roles.h"
#include "session.h"

#include <QDebug>
#include <QtConcurrent>

MessageListModel::MessageListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_HeaderList = new QList<Message *>();

    auto session = Session::getInstance();

    connect(session, &Session::accountAdded, this, &MessageListModel::addAccount);

    connect(this, &MessageListModel::onMessagesReady, this, &MessageListModel::addMessages);
    connect(this, &MessageListModel::onMessageReady, this, &MessageListModel::addMessage);
}

int MessageListModel::rowCount(const QModelIndex &parent) const
{
    return m_HeaderList->length();
}

QVariant MessageListModel::data(const QModelIndex &index, int role) const
{
    Message *header = m_HeaderList->at(index.row());

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
        return parseDate(header->DateTime());
    case Roles::UIDRole:
        return QVariant::fromValue(header->Uid());
    case Roles::EmailRole:
        return header->AccountEmail();
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
    roles[Roles::SelectedRole] = "Selected";
    return roles;
}

void MessageListModel::addAccount(Account *account)
{
    m_Accounts.append(account);

    connect(account, &Account::folderSelected, this, &MessageListModel::onFolderSelected);
}

void MessageListModel::selectedFolder(const QString &email, const QString &foldername)
{
    emit messageLoading();
    if (m_Thread.isRunning()) {
        m_ThreadRunning = false;
        m_Thread.cancel();
    }
    m_Thread = QtConcurrent::run(this, &MessageListModel::loadMessages, email, foldername);
}

void MessageListModel::deleteMessages(QModelIndexList selectedList)
{
    QList<ssize_t> uidList;
    QMap<QString, QList<ssize_t>> accountUidList;

    for (int i = 0; i < m_Accounts.length(); i++) {
        accountUidList.insert(m_Accounts.at(i)->Email(), uidList);
    }

    for (int i = 0; i < selectedList.length(); i++) {
        Message *msg = m_HeaderList->at(selectedList.at(i).row());
        accountUidList[msg->AccountEmail()].append(msg->Uid());
    }

    qDebug() << accountUidList;

    bool result = false;

    for (int i = 0; i < m_Accounts.length(); i++) {
        uidList = accountUidList.value(m_Accounts.at(i)->Email());

        if (!uidList.isEmpty()) {
            result = m_Accounts.at(i)->deleteMessage(uidList);
        }
    }

    if (result) {
        for (int i = 0; i < selectedList.length(); i++) {
            m_HeaderList->removeAt(selectedList.at(i).row());
        }
    }
}

void MessageListModel::moveMessage(const QString &sourceFoldername, const QString &destFolderName, QList<ssize_t> uidList)
{
}

void MessageListModel::onFolderSelected(QString foldername)
{
    setLoading(true);
    Account *account = static_cast<Account *>(sender());
    if (m_Thread.isRunning()) {
        m_ThreadRunning = false;
        m_Thread.cancel();
    }
    m_HeaderList->clear();
    m_Thread = QtConcurrent::run(this, &MessageListModel::loadMessages, account->Email(), foldername);
}

void MessageListModel::parseMessages(Account *account, QMap<ssize_t, QString> headerData)
{
    QMapIterator<ssize_t, QString> iter(headerData);
    QList<Message *> *msgList = new QList<Message *>();

    while (iter.hasNext()) {
        iter.next();

        Message *msg = new Message(account->Email(), iter.key());
        msg->setHeaderData(iter.value());
        msgList->append(msg);

        //        emit onMessageReady(msg);
    }

    emit onMessagesReady(msgList);
    emit messageLoadingFinished();
}

void MessageListModel::addMessages(QList<Message *> *msgList)
{
    if (msgList->isEmpty())
        return;

    beginInsertRows(QModelIndex(), rowCount(QModelIndex()), msgList->count() - 1 + rowCount(QModelIndex()));
    m_HeaderList->append(*msgList);
    endInsertRows();
}

void MessageListModel::addMessage(Message *msg)
{
    beginInsertRows(QModelIndex(), rowCount(QModelIndex()), 1 + rowCount(QModelIndex()));
    m_HeaderList->append(msg);
    endInsertRows();
}

void MessageListModel::loadMessages(const QString &email, const QString &foldername)
{
    if (email.isEmpty()) {
        return;
    }

    m_ThreadRunning = true;

    for (auto account : qAsConst(m_Accounts)) {
        if (account->Email() == email) {
            QMap<ssize_t, QString> headerData;
            bool result = account->getHeaders(foldername, headerData);
            if (result)

            {
                parseMessages(account, headerData);
            }
            break;
        }
    }

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
    if (m_loading == newLoading) {
        return;
    }

    m_loading = newLoading;
    emit loadingChanged();
}
