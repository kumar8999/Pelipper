#ifndef FLAGS_H
#define FLAGS_H

#include <QObject>

class Flags : public QObject
{
    Q_OBJECT
public:
    explicit Flags(QObject *parent = nullptr);

    bool seenFlag() const;
    void setSeenFlag(bool newSeenFlag);

    bool deletedFlag() const;
    void setDeletedFlag(bool newDeletedFlag);

    bool answeredFlag() const;
    void setAnsweredFlag(bool newAnsweredFlag);

    bool raftFlag() const;
    void setDraftFlag(bool newDraftFlag);

    bool recentFlag() const;
    void setRecentFlag(bool newRecentFlag);

private:
    bool m_seenFlag;
    bool m_deletedFlag;
    bool m_answeredFlag;
    bool m_draftFlag;
    bool m_recentFlag;
};

#endif // FLAGS_H
