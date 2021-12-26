#include "application.h"

#include "roles.h"
#include "session.h"

Application::Application(QObject *parent)
    : m_isAccountInit(false)
    , m_hasMsgInited(false)
    , QObject(parent)
{
    m_Settings = new Settings(this);
    m_folderListModel = new FolderListModel(this);
    m_messageListModel = new SortModel(this);
    MessageListModel *_messageListModel = new MessageListModel(this);
    m_messageListModel->setSourceModel(_messageListModel);
    m_messageListModel->setSortRole(Roles::DateRole);
    m_messageListModel->setDynamicSortFilter(true);
    m_messageListModel->sort(0, Qt::DescendingOrder);

    connect(_messageListModel, &MessageListModel::loadingChanged, m_messageListModel, &SortModel::onLoadingChanged);

    connect(_messageListModel, &MessageListModel::messageLoading, this, [=]() {
        //                auto _temp = static_cast<MessageListFilter *>(m_messageListModel);
        //                _temp->setMessageLoading(true);
    });

    connect(_messageListModel, &MessageListModel::messageLoadingFinished, this, [=]() {
        //        auto _temp = static_cast<MessageListFilter *>(m_messageListModel);
        //        _temp->setMessageLoading(false);
    });
    m_message = new MessageItem(this);
    setHasMsgInited(false);

    loadAccounts();
}

FolderListModel *Application::folderListModel() const
{
    return m_folderListModel;
}

bool Application::isAccountInit() const
{
    return m_isAccountInit;
}

void Application::setIsAccountInit(bool newIsAccountInit)
{
    if (m_isAccountInit == newIsAccountInit)
        return;
    m_isAccountInit = newIsAccountInit;
    emit isAccountInitChanged();
}

void Application::loadAccounts()
{
    QStringList accounts = m_Settings->getAccountIds();

    for (const auto &accountId : qAsConst(accounts)) {
        QString username;
        QString email = accountId;
        QString password;
        QString imapServer;
        int imapPort;
        QString smtpServer;
        int smtpPort;
        bool isOK = m_Settings->getAccountSettings(username, email, password, imapServer, imapPort, smtpServer, smtpPort);
        if (isOK) {
            Account *account = new Account(username, email, password, imapServer, imapPort, smtpServer, smtpPort);
            Session::getInstance()->addAccount(account);
            setIsAccountInit(true);
            m_Accounts.insert(email, account);
        }
    }
}

SortModel *Application::messageListModel() const
{
    return m_messageListModel;
}

void Application::setMessageListModel(SortModel *newMessageListModel)
{
    if (m_messageListModel == newMessageListModel)
        return;
    m_messageListModel = newMessageListModel;
    emit messageListModelChanged();
}

void Application::setFolderListModel(FolderListModel *newFolderListModel)
{
    if (m_folderListModel == newFolderListModel)
        return;
    m_folderListModel = newFolderListModel;
    emit folderListModelChanged();
}

void Application::selectedMessage(QString email, long uid)
{
    Account *account = m_Accounts.value(email);

    QString data;
    account->getBody(uid, data);

    Message *msg = new Message(email, uid, this);
    msg->setBodyData(data);
    MessageItem *_message = new MessageItem(this);
    _message->setMessage(msg);
    setMessage(_message);
}

bool Application::hasMsgInited() const
{
    return m_hasMsgInited;
}

void Application::setHasMsgInited(bool newHasMsgInited)
{
    if (m_hasMsgInited == newHasMsgInited)
        return;
    m_hasMsgInited = newHasMsgInited;
    emit hasMsgInitedChanged();
}

MessageItem *Application::message() const
{
    return m_message;
}

void Application::setMessage(MessageItem *newMessage)
{
    if (m_message == newMessage)
        return;

    m_message = newMessage;
    setHasMsgInited(true);
    emit messageChanged();
}
