#include "address.h"

Address::Address(QObject *parent)
    : QObject{parent}
{
}

const QString &Address::displayName() const
{
    return m_displayName;
}

void Address::setDisplayName(const QString &newDisplayName)
{
    m_displayName = newDisplayName;
}

void Address::setAddress(const QString &newAddress)
{
    m_address = newAddress;
}

const QString &Address::address() const
{
    return m_address;
}
