#ifndef MESSAGELISTMODEL_H
#define MESSAGELISTMODEL_H

#include "account.h"
#include "message.h"

#include <QAbstractListModel>
#include <QFuture>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>

class MessageListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)

public:
    explicit MessageListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;

    void loadFolder();

    void addAccount(Account *account);

    void selectedFolder(const QString &email, const QString &foldername);

    void deleteMessages(QModelIndexList selectedList);

    void moveMessage(const QString &sourceFoldername, const QString &destFolderName, QList<ssize_t> uidList);

    bool loading() const;

    void setLoading(bool newLoading);
signals:
    void onMessagesReady(QList<Message *> *msgList);

    void onMessageReady(Message *msg);

    void messageLoading();

    void messageLoadingFinished();

    void loadingChanged();

private slots:
    void onFolderSelected(QString foldername);

    void addMessages(QList<Message *> *msgList);

    void addMessage(Message *msg);

private:
    void parseMessages(Account *account, QMap<ssize_t, QString> headerData);

    void loadMessages(const QString &email, const QString &foldername);

    QString parseDate(const QDateTime &datetime) const;

private:
    QList<Message *> *m_HeaderList;
    QList<Account *> m_Accounts;
    QFuture<void> m_Thread;

    bool m_loading = false;

    bool m_ThreadRunning = false;
    Q_PROPERTY(bool loading READ loading CONSTANT)
};

#endif // MESSAGELISTMODEL_H
