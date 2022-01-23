#ifndef MESSAGELISTMODEL_H
#define MESSAGELISTMODEL_H

#include "../backend/account.h"
#include "../backend/message.h"
#include "roles.h"

#include <QAbstractListModel>
#include <QFuture>

class MessageListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit MessageListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    void appendRows(QList<Message *> *messageList);

    bool loading() const;
    void setLoading(bool newLoading);

public slots:
    void onFolderSelected(QHash<Account *, Folder *> accountFolder);

signals:
    void loadingChanged();

    void messageReadyFinished(QList<Message *> *msgList);

private slots:
    void onMessageReadyFinished(QList<Message *> *msgList);

private:
    void loadMessage(Account *account, Folder *folder);

    QString parseDate(const QDateTime &datetime) const;

private:
    QList<Message *> *m_MessageList;

    bool m_loading;
    bool m_ThreadRunning = false;

    QList<QFuture<void>> m_Threads;

    QFuture<void> m_Thread;
};

#endif // MESSAGELISTMODEL_H
