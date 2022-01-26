#include "flags.h"

Flags::Flags(QObject *parent)
    : m_SeenFlag(false)
    , m_deletedFlag(false)
    , m_AnsweredFlag(false)
    , m_DraftFlag(false)
    , m_RecentFlag(false)
    , QObject{parent}
{
}

bool Flags::SeenFlag() const
{
    return m_SeenFlag;
}

void Flags::setSeenFlag(bool newSeenFlag)
{
    m_SeenFlag = newSeenFlag;
}

bool Flags::deletedFlag() const
{
    return m_deletedFlag;
}

void Flags::setDeletedFlag(bool newDeletedFlag)
{
    m_deletedFlag = newDeletedFlag;
}

bool Flags::AnsweredFlag() const
{
    return m_AnsweredFlag;
}

void Flags::setAnsweredFlag(bool newAnsweredFlag)
{
    m_AnsweredFlag = newAnsweredFlag;
}

bool Flags::DraftFlag() const
{
    return m_DraftFlag;
}

void Flags::setDraftFlag(bool newDraftFlag)
{
    m_DraftFlag = newDraftFlag;
}

bool Flags::RecentFlag() const
{
    return m_RecentFlag;
}

void Flags::setRecentFlag(bool newRecentFlag)
{
    m_RecentFlag = newRecentFlag;
}
