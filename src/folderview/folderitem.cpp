#include "folderitem.h"

FolderItem::FolderItem(Account *account, Folder *folder)
{
    m_account = account;
    m_folder = folder;
    setText(folder->DisplayName());
}

QString FolderItem::Type() const
{
    return "FolderItem";
}

Account *FolderItem::account() const
{
    return m_account;
}

Folder *FolderItem::folder() const
{
    return m_folder;
}
