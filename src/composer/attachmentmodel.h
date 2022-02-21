#ifndef ATTACHMENTMODEL_H
#define ATTACHMENTMODEL_H

#include <QAbstractItemModel>

class AttachmentModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QStringList AttachmentList READ AttachmentList WRITE setAttachmentList NOTIFY AttachmentListChanged)

    enum { FileNameRole = Qt::UserRole + 1, IconRole, FileSource };

public:
    explicit AttachmentModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    const QStringList &AttachmentList() const;
    void setAttachmentList(const QStringList &newAttachmentList);

signals:
    void error();

    void AttachmentListChanged();

public slots:
    void addAttachmentFile(QString filePath);

private:
    QStringList m_AttachmentList;
};

#endif // ATTACHMENTMODEL_H
