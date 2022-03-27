#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QObject>

enum MessageOperation { MoveOperation, DeleteOperation };

class MessageHandler : public QObject
{
    Q_OBJECT
public:
    explicit MessageHandler(QObject *parent = nullptr);

    void deleteMessages();

signals:
};

#endif // MESSAGEHANDLER_H
