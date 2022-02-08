#include "attachmentmodel.h"

#include <QDir>
#include <QFileInfo>
#include <QIcon>
#include <QMimeDatabase>
#include <QPixmap>

AttachmentModel::AttachmentModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int AttachmentModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    return m_AttachmentList.length();
}

QVariant AttachmentModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    QString sourceFile = m_AttachmentList.at(index.row());

    switch (role) {
    case FileNameRole: {
        QFileInfo fileinfo(sourceFile);
        return fileinfo.baseName();
    }
    case IconRole: {
        QMimeDatabase mime_database;
        const QString nativeName = QDir::toNativeSeparators(sourceFile);
        QIcon icon;
        QList<QMimeType> mime_types = mime_database.mimeTypesForFileName(nativeName);
        icon = QIcon::fromTheme(mime_types[0].iconName());
        return icon.pixmap(16);
    }
    case FileSource:
        return sourceFile;
    }

    return QVariant();
}

QHash<int, QByteArray> AttachmentModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[FileNameRole] = "filename";
    roles[IconRole] = "icon";
    roles[FileSource] = "source";
    return roles;
}

void AttachmentModel::addAttachmentFile(QString filePath)
{
    QFileInfo fileinfo(filePath);

    if (fileinfo.exists()) {
        m_AttachmentList.append(filePath);
    } else {
        emit error();
    }
}

const QStringList &AttachmentModel::AttachmentList() const
{
    return m_AttachmentList;
}

void AttachmentModel::setAttachmentList(const QStringList &newAttachmentList)
{
    if (m_AttachmentList == newAttachmentList)
        return;
    m_AttachmentList = newAttachmentList;
    emit AttachmentListChanged();
}
