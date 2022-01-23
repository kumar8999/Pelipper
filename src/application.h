#ifndef APPLICATION_H
#define APPLICATION_H

#include "backend/session.h"
#include "backend/settings.h"
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

    Q_INVOKABLE void selectedMessage(QString accountEmail, int uid);

public slots:
    void addAccount(const QString &username,
                    const QString &email,
                    const QString &password,
                    const QString &imapServer,
                    const int &imapPort,
                    const QString &smtpServer,
                    const int &smtpPort);

signals:
    void folderListModelChanged();

    void isAccountInitChanged();

    void connectionSuccessFul(bool isOk);

    void loginSuccessFul(bool isOk);

    void hasMsgLoadedChanged();

    void messageListModelChanged();

    void messageItemChanged();

    void messageReadReady(Message *msg);

private:
    void loadAccounts();

private:
    bool m_isAccountInit;
    bool m_hasMsgLoaded;

    Settings *m_Settings;

    FolderListModel *m_folderListModel;
    SortModel *m_messageListModel;
    MessageItem *m_messageItem;
};

#endif // APPLICATION_H
