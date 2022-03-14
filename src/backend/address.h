#ifndef ADDRESS_H
#define ADDRESS_H

#include <QObject>

class Address : public QObject
{
    Q_OBJECT
public:
    explicit Address(QObject *parent = nullptr);

    const QString &displayName() const;
    void setDisplayName(const QString &newDisplayName);

    void setAddress(const QString &newAddress);
    const QString &address() const;

private:
    QString m_displayName;
    QString m_address;
};

#endif // ADDRESS_H
