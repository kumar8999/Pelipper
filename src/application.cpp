#include "application.h"

#include "backend/messagehandler.h"

#include <QtConcurrent>

Application::Application(QObject *parent)
    : m_isAccountInit(false)
    , m_messageItem(nullptr)
    , m_hasMsgLoaded(false)
    , QObject(parent)
{
    m_Settings = new Settings(this);
    m_folderListModel = new FolderListModel(this);
    m_folderHandler = new FolderHandler(this);
    m_folderListModel->setFolderhandler(m_folderHandler);
    m_syncManager = new SyncManager(this);
    m_folderHandler->setSyncmanager(m_syncManager);

    MessageListModel *_messageListModel = new MessageListModel(this);
    MessageHandler *messageHandler = new MessageHandler(this);
    _messageListModel->setMessageHandler(messageHandler);
    messageHandler->setSyncmanager(m_syncManager);

    m_messageListModel = new SortModel(this);
    m_messageListModel->setModel(_messageListModel);
    m_messageListModel->setSortRole(Roles::DateRole);
    m_messageListModel->sort(0, Qt::DescendingOrder);

    //    m_attachmentListModel = new AttachmentModel(this);

    connect(m_folderListModel, &FolderListModel::folderSelected, _messageListModel, &MessageListModel::onFolderSelected);

    connect(this, &Application::messageReadReady, this, [=](Message *msg) {
        setMessageItem(new MessageItem(msg, this));
        setHasMsgLoaded(true);
    });

    connect(m_syncManager, &SyncManager::messageReadFinished, this, [=](Account *account, Message *msg) {
        setMessageItem(new MessageItem(msg, this));
        setHasMsgLoaded(true);
    });

    loadAccounts();
}

Application::~Application()
{
    delete m_folderListModel;
    delete m_messageListModel;
    delete m_attachmentListModel;
    delete m_Settings;

    if (m_messageItem != nullptr)
        delete m_messageItem;
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

        // if not connected or log in show an error message and dialog to rewrite details

        accountService->connect();
        accountService->login();
        Session::getInstance()->addAccount(account);
        setIsAccountInit(true);
    }
}

AttachmentModel *Application::attachmentListModel() const
{
    return m_attachmentListModel;
}

void Application::setAttachmentListModel(AttachmentModel *newAttachmentListModel)
{
    if (m_attachmentListModel == newAttachmentListModel)
        return;
    m_attachmentListModel = newAttachmentListModel;
    emit attachmentListModelChanged();
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

void Application::selectedMessage(const QString &accountEmail, const QString &foldername, int uid)
{
    QtConcurrent::run([=]() {
        Session *session = Session::getInstance();
        Account *account = session->getAccount(accountEmail);

        ImapService *imapService = account->IMAPService();
        Message *msg = imapService->getBody(uid);

        emit messageReadReady(msg);
    });
}

void Application::sendMessage(const QString &from,
                              const QString &to,
                              const QString &cc,
                              const QString &bcc,
                              const QString &subject,
                              const QString &msg,
                              const QStringList &attachments)
{
    QtConcurrent::run([=]() {
        Session *session = Session::getInstance();
        Account *account = session->getAccount(from);
        SmtpService *smtpService = account->SMTPService();

        QStringList toList = to.split(",");
        QList<Contact> toContactList;

        for (auto i = 0; i < toList.length(); i++) {
            Contact contact(toList.at(i));
            toContactList.append(contact);
        }

        QStringList ccList = cc.split(",");
        QList<Contact> ccContactList;

        for (auto i = 0; i < ccList.length(); i++) {
            Contact contact(ccList.at(i));
            ccContactList.append(contact);
        }

        QStringList bccList = cc.split(",");
        QList<Contact> bccContactList;

        for (auto i = 0; i < bccList.length(); i++) {
            Contact contact(bccList.at(i));
            bccContactList.append(contact);
        }

        smtpService->Send(subject, msg, toContactList, ccContactList, bccContactList, attachments);
    });
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

QStringList Application::accountEmail()
{
    QList<Account *> accounts = Session::getInstance()->getAccounts();

    QStringList accountEmail;
    for (int i = 0; i < accounts.length(); i++) {
        QString email = accounts.at(i)->Email();
        accountEmail.append(email);
    }

    return accountEmail;
}

void Application::onAccountAdded(Account *account)
{
    //    account->startCacheService();
}
