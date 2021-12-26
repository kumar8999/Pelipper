#include "contact.h"

Contact::Contact()
    : m_Address("")
    , m_Hostname("")
{
}

Contact::Contact(const QString &address, const QString &hostname)
    : m_Address(address)
    , m_Hostname(hostname)
{
}

const QString &Contact::Address() const
{
    return m_Address;
}

void Contact::setAddress(const QString &newAddress)
{
    m_Address = newAddress;
}

const QString &Contact::Hostname() const
{
    return m_Hostname;
}

void Contact::setHostname(const QString &newHostname)
{
    m_Hostname = newHostname;
}
