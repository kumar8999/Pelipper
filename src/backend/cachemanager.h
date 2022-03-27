#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <QObject>

class CacheManager : public QObject
{
    Q_OBJECT
public:
    explicit CacheManager(QObject *parent = nullptr);

signals:

private:
};

#endif // CACHEMANAGER_H
