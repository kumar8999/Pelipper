#ifndef ROLES_H
#define ROLES_H

#include <QObject>

class Roles : public QObject
{
    Q_OBJECT
    Q_ENUMS(RoleNames)
public:
    using QObject::QObject;
    ~Roles() = default;
    enum RoleNames { SenderRole = Qt::UserRole + 1, SubjectRole, DateRole, UIDRole, EmailRole, SelectedRole, MessageSeenRole, MessageRecentRole, FolderRole };
};

#endif // ROLES_H
