#ifndef APPLICATION_H
#define APPLICATION_H

#include "backend/folderhandler.h"
#include "backend/session.h"
#include "backend/settings.h"
#include "backend/syncmanager.h"
#include "composer/attachmentmodel.h"
#include "folderview/folderlistmodel.h"
#include "messagelistview/messagelistmodel.h"
#include "messagelistview/sortmodel.h"
#include "messageview/messageitem.h"

#include <QObject>

class Application : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isAccountInit READ isAccountInit WRITE setIsAccountInit NOTIFY isAccountInitChanged)
    Q_PROPERTY(FolderListModel *folderListModel READ folderListModel WRITE setfolderListModel NOTIFY folderListModelChanged)
    Q_PROPERTY(bool hasMsgLoaded READ hasMsgLoaded WRITE setHasMsgLoaded NOTIFY hasMsgLoadedChanged)
    Q_PROPERTY(SortModel *messageListModel READ messageListModel WRITE setMessageListModel NOTIFY messageListModelChanged)
    Q_PROPERTY(MessageItem *messageItem READ messageItem WRITE setMessageItem NOTIFY messageItemChanged)
    Q_PROPERTY(AttachmentModel *attachmentListModel READ attachmentListModel WRITE setAttachmentListModel NOTIFY attachmentListModelChanged)

public:
    Application(QObject *parent = nullptr);
    ~Application();

    FolderListModel *folderListModel() const;
    void setfolderListModel(FolderListModel *newFolderListModel);

    bool isAccountInit() const;
    void setIsAccountInit(bool newIsAccountInit);

    bool hasMsgLoaded() const;
    void setHasMsgLoaded(bool newHasMsgLoaded);

    SortModel *messageListModel() const;
    void setMessageListModel(SortModel *newMessageListModel);

    MessageItem *messageItem() const;
    void setMessageItem(MessageItem *newMessageItem);

    AttachmentModel *attachmentListModel() const;
    void setAttachmentListModel(AttachmentModel *newAttachmentListModel);

public slots:
    void selectedMessage(const QString &accountEmail, const QString &foldername, int uid);

    void sendMessage(const QString &from,
                     const QString &to,
                     const QString &cc,
                     const QString &bcc,
                     const QString &subject,
                     const QString &msg,
                     const QStringList &attachments);

    void addAccount(const QString &username,
                    const QString &email,
                    const QString &password,
                    const QString &imapServer,
                    const int &imapPort,
                    const QString &smtpServer,
                    const int &smtpPort);

    QStringList accountEmail();

signals:
    void folderListModelChanged();

    void isAccountInitChanged();

    void connectionSuccessFul(bool isOk);

    void loginSuccessFul(bool isOk);

    void hasMsgLoadedChanged();

    void messageListModelChanged();

    void messageItemChanged();

    void messageReadReady(Message *msg);

    void attachmentListModelChanged();

private slots:
    //    void onAccountAdded(Account *account);

    void onFolderCacheLoadFinished();

    void onNewMessageRecieved(QString email, QString folder, QList<ssize_t> uidList);

private:
    void loadAccounts();

    void loadCache();

    void startSync();

private:
    bool m_isAccountInit;
    bool m_hasMsgLoaded;

    Settings *m_settings;
    FolderListModel *m_folderListModel;
    SortModel *m_messageListModel;
    MessageItem *m_messageItem;
    AttachmentModel *m_attachmentListModel;
    FolderHandler *m_folderHandler;
    MessageHandler *m_messageHandler;
    SyncManager *m_syncManager;
};

#endif // APPLICATION_H
