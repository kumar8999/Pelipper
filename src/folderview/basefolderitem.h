#ifndef BASEFOLDERITEM_H
#define BASEFOLDERITEM_H

#include <QStandardItem>

class BaseFolderItem : public QStandardItem
{
public:
    BaseFolderItem();

    virtual QString Type() const;
};

#endif // BASEFOLDERITEM_H
