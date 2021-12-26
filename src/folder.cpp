#include "folder.h"

Folder::Folder(const QString &foldername, const QString &fullFoldername, QObject *parent)
    : m_FolderName(foldername)
    , m_FullFolderName(fullFoldername)
    , QObject(parent)
{
}

bool Folder::hasChildren()
{
    return m_Children.length() > 0;
}

const QList<Folder *> &Folder::Children() const
{
    return m_Children;
}

void Folder::appendChild(Folder *item)
{
    m_Children.append(item);
}

const QString &Folder::FolderName() const
{
    return m_FolderName;
}

const QString &Folder::FullFolderName() const
{
    return m_FullFolderName;
}

const QString &Folder::Email() const
{
    return m_Email;
}

void Folder::setEmail(const QString &newEmail)
{
    m_Email = newEmail;
}
