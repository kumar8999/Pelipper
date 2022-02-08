#ifndef FOLDER_H
#define FOLDER_H

#include <QObject>

class Folder : public QObject
{
    Q_OBJECT
public:
    Folder(const QString &fullname, const QChar &delimter, QObject *parent = nullptr);

    bool hasChildren();

    const QList<Folder *> &Children() const;

    void appendChild(Folder *item);

    const QString &DisplayName() const;

    const QString &FullName() const;

    const QChar &Delimiter() const;

private:
    QString m_DisplayName;
    QString m_FullName;
    QChar m_Delimiter;

    QList<Folder *> m_Children;
};

#endif // FOLDER_H
