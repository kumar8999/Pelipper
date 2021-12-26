#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = nullptr);

    bool saveAccountSettings(const QString &username,
                             const QString &email,
                             const QString &password,
                             const QString &imapServer,
                             const int &imapPort,
                             const QString &smtpServer,
                             const int &smtpPort);

    bool getAccountSettings(QString &username, const QString &email, QString &password, QString &imapServer, int &imapPort, QString &smtpServer, int &smtpPort);

    QStringList getAccountIds();

    bool isAccountCached(const QString &accountEmail);

    void saveIsAccountCached(const QString &accountEmail, bool isAccountCached);

private:
    QSettings *m_Settings;
};

#endif // SETTINGS_H
