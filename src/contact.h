#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>

class Contact
{
public:
    Contact();
    Contact(const QString &address, const QString &hostname = "");

    const QString &Address() const;
    void setAddress(const QString &newAddress);

    const QString &Hostname() const;
    void setHostname(const QString &newHostname);

private:
    QString m_Address = "";
    QString m_Hostname = "";
};

#endif // CONTACT_H
