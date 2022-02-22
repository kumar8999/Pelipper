#ifndef MESSAGELISTMODEL_H
#define MESSAGELISTMODEL_H

#include "../Threads/messagelistthread.h"
#include "../backend/account.h"
#include "../backend/message.h"
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

public slots:
    void onFolderSelected(QHash<Account *, Folder *> *accountFolder);

    void setSeenFlag(int row);

signals:
    void loadingChanged();

    void messageReadReady(QList<Message *> *msgList);

private slots:
    void onMessageReadyFinished(QList<Message *> *msgList);

private:
    QString parseDate(const QDateTime &datetime) const;

    void loadMessage(QHash<Account *, Folder *> *accountFolder);

private:
    QList<Message *> *m_messageList;

    bool m_loading;

    QFuture<void> m_future;
};

#endif // MESSAGELISTMODEL_H
