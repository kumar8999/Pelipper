#include "settings.h"

#include <QDebug>
#include <QDir>
#include <QStandardPaths>

Settings::Settings(QObject *parent)
    : QObject(parent)
{
    QDir dir;
    dir.mkpath(QDir::homePath() + "/.config/Pelipper");

    QStringList configPaths = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation);
    QString configPath = configPaths[0] + "/";

    qDebug() << configPaths;

    m_Settings = new QSettings(configPath + "/Pelipper/config", QSettings::IniFormat, this);
}

bool Settings::saveAccountSettings(const QString &username,
                                   const QString &email,
                                   const QString &password,
                                   const QString &imapServer,
                                   const int &imapPort,
                                   const QString &smtpServer,
                                   const int &smtpPort)
{
    QStringList accountIds = m_Settings->value("account.id", QStringList()).toStringList();
    if (accountIds.contains(email)) {
        return false;
    }

    qDebug() << accountIds;

    accountIds.append(email);

    qDebug() << email;

    m_Settings->setValue("account.id", accountIds);
    m_Settings->beginGroup("account.settings." + email);
    m_Settings->setValue("imap.server", imapServer);
    m_Settings->setValue("imap.port", imapPort);
    m_Settings->setValue("smtp.server", smtpServer);
    m_Settings->setValue("smtp.port", smtpPort);
    m_Settings->setValue("account.email", email);
    m_Settings->setValue("account.username", username);
    m_Settings->setValue("account.password", password);
    m_Settings->endGroup();

    return true;
}

bool Settings::getAccountSettings(QString &username,
                                  const QString &email,
                                  QString &password,
                                  QString &imapServer,
                                  int &imapPort,
                                  QString &smtpServer,
                                  int &smtpPort)
{
    QStringList accountIds = m_Settings->value("account.id", QStringList()).toStringList();
    if (!accountIds.contains(email)) {
        return false;
    }

    m_Settings->beginGroup("account.settings." + email);
    imapServer = m_Settings->value("imap.server").toString();
    imapPort = m_Settings->value("imap.port").toInt();
    smtpServer = m_Settings->value("smtp.server").toString();
    smtpPort = m_Settings->value("smtp.port").toInt();
    username = m_Settings->value("account.username").toString();
    password = m_Settings->value("account.password").toString();
    m_Settings->endGroup();

    return true;
}

QStringList Settings::getAccountIds()
{
    QStringList accountIds = m_Settings->value("account.id", QStringList()).toStringList();
    return accountIds;
}

bool Settings::isAccountCached(const QString &accountEmail)
{
    bool isAccountCached = false;
    m_Settings->beginGroup("account.settings." + accountEmail);
    isAccountCached = m_Settings->value("account.cached", false).toBool();
    m_Settings->endGroup();

    return isAccountCached;
}

void Settings::saveIsAccountCached(const QString &accountEmail, bool isAccountCached)
{
    m_Settings->beginGroup("account.settings." + accountEmail);
    m_Settings->setValue("account.cached", isAccountCached);
    m_Settings->endGroup();
}
