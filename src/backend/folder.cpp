#include "folder.h"

Folder::Folder(const QString &fullname, const QChar &delimter, QObject *parent)
    : m_FullName(fullname)
    , m_Delimiter(delimter)
{
    QStringList split = m_FullName.split(delimter);
    m_DisplayName = split.at(split.length() - 1);
}

bool Folder::hasChildren()
{
    return m_Children.length() > 0;
}

const QList<Folder *> &Folder::Children() const
{
    return m_Children;
}

void Folder::appendChild(Folder *item)
{
    m_Children.append(item);
}

const QString &Folder::DisplayName() const
{
    return m_DisplayName;
}

const QString &Folder::FullName() const
{
    return m_FullName;
}

const QChar &Folder::Delimiter() const
{
    return m_Delimiter;
}

Folder *Folder::Parent() const
{
    return m_Parent;
}

void Folder::setParent(Folder *newParent)
{
    m_Parent = newParent;
}

void Folder::setDisplayName(const QString &newDisplayName)
{
    m_DisplayName = newDisplayName;
}
