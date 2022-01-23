#include "folderitem.h"

FolderItem::FolderItem(Account *account, Folder *folder)
{
    m_Account = account;
    m_Folder = folder;
    setText(folder->DisplayName());
}

QString FolderItem::Type() const
{
    return "FolderItem";
}

Account *FolderItem::account() const
{
    return m_Account;
}

Folder *FolderItem::folder() const
{
    return m_Folder;
}
