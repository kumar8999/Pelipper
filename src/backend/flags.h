#ifndef FLAGS_H
#define FLAGS_H

#include <QObject>

class Flags : public QObject
{
    Q_OBJECT
public:
    explicit Flags(QObject *parent = nullptr);

    bool SeenFlag() const;
    void setSeenFlag(bool newSeenFlag);

    bool deletedFlag() const;
    void setDeletedFlag(bool newDeletedFlag);

    bool AnsweredFlag() const;
    void setAnsweredFlag(bool newAnsweredFlag);

    bool DraftFlag() const;
    void setDraftFlag(bool newDraftFlag);

    bool RecentFlag() const;
    void setRecentFlag(bool newRecentFlag);

private:
    bool m_SeenFlag;
    bool m_deletedFlag;
    bool m_AnsweredFlag;
    bool m_DraftFlag;
    bool m_RecentFlag;
};

#endif // FLAGS_H
