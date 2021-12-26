#ifndef FOLDER_H
#define FOLDER_H

#include <QObject>

class Folder : public QObject
{
    Q_OBJECT
public:
    explicit Folder(const QString &foldername, const QString &fullFoldername, QObject *parent = nullptr);

    bool hasChildren();

    const QList<Folder *> &Children() const;

    void appendChild(Folder *item);

    void select();

    const QString &FolderName() const;

    const QString &FullFolderName() const;

    const QString &Email() const;
    void setEmail(const QString &newEmail);

private:
    QString m_FolderName;
    QString m_FullFolderName;
    QString m_Email;

    QList<Folder *> m_Children;
};

#endif // FOLDER_H
