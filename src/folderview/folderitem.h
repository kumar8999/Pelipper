#ifndef FOLDERITEM_H
#define FOLDERITEM_H

#include "../backend/account.h"
#include "basefolderitem.h"

class FolderItem : public BaseFolderItem
{
public:
    FolderItem(Account *account, Folder *folder);

    FolderItem(Folder *folder);

    QString Type() const override;

    Account *account() const;

    Folder *folder() const;

private:
    Account *m_account;
    Folder *m_folder;
};
#endif // FOLDERITEM_H
