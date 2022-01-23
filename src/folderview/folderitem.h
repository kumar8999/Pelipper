#ifndef FOLDERITEM_H
#define FOLDERITEM_H

#include "../backend/account.h"
#include "basefolderitem.h"

class FolderItem : public BaseFolderItem
{
public:
    FolderItem(Account *account, Folder *folder);

    QString Type() const override;

    Account *account() const;

    Folder *folder() const;

private:
    Account *m_Account;
    Folder *m_Folder;
};
#endif // FOLDERITEM_H
