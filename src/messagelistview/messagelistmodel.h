#ifndef MESSAGELISTMODEL_H
#define MESSAGELISTMODEL_H

#include "../Threads/messagelistthread.h"
#include "../backend/account.h"
#include "../backend/message.h"
#include "../backend/messagehandler.h"
#include "roles.h"

#include <QAbstractListModel>
#include <QFuture>

class MessageListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)

public:
    explicit MessageListModel(QObject *parent = nullptr);

    ~MessageListModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    void appendRows(QList<Message *> *messageList);

    bool loading() const;

    void setLoading(bool newLoading);

    void deleteMessages(QModelIndexList indexList);

    void moveMessages(QModelIndexList indexList, const QString &destDir);

    void setMessageHandler(MessageHandler *newMessageHandler);

    void selectMessage(QModelIndex index);

public slots:
    void onFolderSelected(QHash<Account *, QString> *accountFolder);

    void setSeenFlag(int row);

signals:
    void loadingChanged();

    void messageReadReady(QList<Message *> *msgList);

    void deleteMessage(QModelIndexList indexList);

private slots:
    void onMessageReadyFinished(QList<Message *> *msgList);

    void onDeleteMessage(QModelIndexList indexList);

    void onMessageLoadFinished();

private:
    QString parseDate(const QDateTime &datetime) const;

    void _deleteMessages(QModelIndexList indexList);

    void _moveMessages(QModelIndexList indexList, const QString &destDir);

private:
    QList<Message *> *m_messageList;
    QMap<ssize_t, QString> folders;

    MessageHandler *m_messageHandler;

    bool m_loading;

    QFuture<void> m_future;
};

#endif // MESSAGELISTMODEL_H
