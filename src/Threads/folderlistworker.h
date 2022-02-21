#ifndef FOLDERLISTWORKER_H
#define FOLDERLISTWORKER_H

#include <QObject>

class FolderListWorker : public QObject
{
    Q_OBJECT
public:
    explicit FolderListWorker(QObject *parent = nullptr);

signals:

private:
};

#endif // FOLDERLISTWORKER_H
