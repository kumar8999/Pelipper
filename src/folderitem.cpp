#include "folderitem.h"

FolderItem::FolderItem(const QString &email, const QString &folderName, const QString &fullFoldername)
    : m_Email(email)
    , m_FolderName(folderName)
    , m_FullFolderName(fullFoldername)

{
    setText(folderName);
}

QString FolderItem::Type() const
{
    return "FolderItem";
}

const QString &FolderItem::Email() const
{
    return m_Email;
}

const QString &FolderItem::FolderName() const
{
    return m_FolderName;
}

const QString &FolderItem::FullFolderName() const
{
    return m_FullFolderName;
}
