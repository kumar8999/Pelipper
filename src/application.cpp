#include "application.h"

#include <QtConcurrent>

Application::Application(QObject *parent)
    : m_isAccountInit(false)
    , m_messageItem(nullptr)
    , m_hasMsgLoaded(false)
    , QObject(parent)
{
    m_Settings = new Settings(this);
    m_folderListModel = new FolderListModel(this);
    MessageListModel *_messageListModel = new MessageListModel(this);

    m_messageListModel = new SortModel(this);
    m_messageListModel->setModel(_messageListModel);
    m_messageListModel->setSortRole(Roles::DateRole);
    m_messageListModel->sort(0, Qt::DescendingOrder);

    connect(m_folderListModel, &FolderListModel::folderSelected, _messageListModel, &MessageListModel::onFolderSelected);
    connect(this, &Application::messageReadReady, this, [=](Message *msg) {
        setMessageItem(new MessageItem(msg, this));
        setHasMsgLoaded(true);
    });
    loadAccounts();
}

Application::~Application()
{
}

FolderListModel *Application::folderListModel() const
{
    return m_folderListModel;
}

void Application::setfolderListModel(FolderListModel *newFolderListModel)
{
    if (m_folderListModel == newFolderListModel)
        return;
    m_folderListModel = newFolderListModel;
    emit folderListModelChanged();
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

        if (!isOK) {
            continue;
        }

        Account *account = new Account(username, email, password, imapServer, imapPort, smtpServer, smtpPort);
        ImapService *accountService = account->IMAPService();
        accountService->connect();
        accountService->login();
        Session::getInstance()->addAccount(account);
        setIsAccountInit(true);
    }
}

MessageItem *Application::messageItem() const
{
    return m_messageItem;
}

void Application::setMessageItem(MessageItem *newMessageItem)
{
    if (m_messageItem == newMessageItem)
        return;
    m_messageItem = newMessageItem;
    emit messageItemChanged();
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

void Application::selectedMessage(QString accountEmail, int uid)
{
    QtConcurrent::run([=]() {
        qDebug() << "selected messaged";
        Session *session = Session::getInstance();
        Account *account = session->getAccount(accountEmail);

        qDebug() << accountEmail;

        ImapService *imapService = account->IMAPService();

        Message *msg = imapService->getBody(uid);

        emit messageReadReady(msg);
    });
}

void Application::saveCache()
{
}

bool Application::hasMsgLoaded() const
{
    return m_hasMsgLoaded;
}

void Application::setHasMsgLoaded(bool newHasMsgLoaded)
{
    if (m_hasMsgLoaded == newHasMsgLoaded)
        return;
    m_hasMsgLoaded = newHasMsgLoaded;
    emit hasMsgLoadedChanged();
}

void Application::addAccount(const QString &username,
                             const QString &email,
                             const QString &password,
                             const QString &imapServer,
                             const int &imapPort,
                             const QString &smtpServer,
                             const int &smtpPort)
{
    Account *account = new Account(username, email, password, imapServer, imapPort, smtpServer, smtpPort);
    ImapService *accountService = account->IMAPService();

    bool result = accountService->connect();

    if (result) {
        emit connectionSuccessFul(true);

        result = accountService->login();
        if (result) {
            Session::getInstance()->addAccount(account);
            m_Settings->saveAccountSettings(username, email, password, imapServer, imapPort, smtpServer, smtpPort);
            setIsAccountInit(true);
            emit loginSuccessFul(true);
        } else {
            emit loginSuccessFul(false);
        }
    } else {
        emit connectionSuccessFul(false);
    }
}
