#ifndef FOLDERITEM_H
#define FOLDERITEM_H

#include "basefolderitem.h"

class FolderItem : public BaseFolderItem
{
public:
    FolderItem(const QString &email, const QString &folderName, const QString &fullFoldername);

    QString Type() const;

    const QString &Email() const;

    const QString &FolderName() const;

    const QString &FullFolderName() const;

private:
    QString m_Email;
    QString m_FolderName;
    QString m_FullFolderName;
};
#endif // FOLDERITEM_H
