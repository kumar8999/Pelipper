#include "flags.h"

Flags::Flags(QObject *parent)
    : m_seenFlag(false)
    , m_deletedFlag(false)
    , m_answeredFlag(false)
    , m_draftFlag(false)
    , m_recentFlag(false)
    , QObject{parent}
{}

bool Flags::seenFlag() const
{
    return m_seenFlag;
}

void Flags::setSeenFlag(bool newSeenFlag)
{
    m_seenFlag = newSeenFlag;
}

bool Flags::deletedFlag() const
{
    return m_deletedFlag;
}

void Flags::setDeletedFlag(bool newDeletedFlag)
{
    m_deletedFlag = newDeletedFlag;
}

bool Flags::answeredFlag() const
{
    return m_answeredFlag;
}

void Flags::setAnsweredFlag(bool newAnsweredFlag)
{
    m_answeredFlag = newAnsweredFlag;
}

bool Flags::draftFlag() const
{
    return m_draftFlag;
}

void Flags::setDraftFlag(bool newDraftFlag)
{
    m_draftFlag = newDraftFlag;
}

bool Flags::recentFlag() const
{
    return m_recentFlag;
}

void Flags::setRecentFlag(bool newRecentFlag)
{
    m_recentFlag = newRecentFlag;
}
