#ifndef APPLICATION_H
#define APPLICATION_H

#include "folderlistmodel.h"
#include "messageitem.h"
#include "messagelistmodel.h"
#include "settings.h"
#include "sortmodel.h"

#include <QObject>

class Application : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isAccountInit READ isAccountInit WRITE setIsAccountInit NOTIFY isAccountInitChanged)
    Q_PROPERTY(MessageItem *message READ message WRITE setMessage NOTIFY messageChanged)
    Q_PROPERTY(bool hasMsgInited READ hasMsgInited WRITE setHasMsgInited NOTIFY hasMsgInitedChanged)
    Q_PROPERTY(SortModel *messageListModel READ messageListModel WRITE setMessageListModel NOTIFY messageListModelChanged)
    Q_PROPERTY(FolderListModel *folderListModel READ folderListModel WRITE setFolderListModel NOTIFY folderListModelChanged)

public:
    explicit Application(QObject *parent = nullptr);

    FolderListModel *folderListModel() const;
    void setfolderListModel(FolderListModel *newFolderListModel);

    bool isAccountInit() const;
    void setIsAccountInit(bool newIsAccountInit);

    void setFolderListModel(FolderListModel *newFolderListModel);

    bool hasMsgInited() const;
    void setHasMsgInited(bool newHasMsgInited);

    MessageItem *message() const;
    void setMessage(MessageItem *newMessage);

    SortModel *messageListModel() const;
    void setMessageListModel(SortModel *newMessageListModel);

public slots:
    void selectedMessage(QString email, long uid);

signals:
    void folderListModelChanged();

    void isAccountInitChanged();

    void htmlChanged();

    void hasMsgInitedChanged();

    void messageChanged();

    void messageListModelChanged();

private:
    void loadAccounts();

private:
    FolderListModel *m_folderListModel;
    bool m_isAccountInit;
    Settings *m_Settings;

    SortModel *m_messageListModel;

    QMap<QString, Account *> m_Accounts;
    MessageItem *m_message;
    bool m_hasMsgInited;
};

#endif // APPLICATION_H
