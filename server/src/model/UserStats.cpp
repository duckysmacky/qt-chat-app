#include "UserStats.h"

#include <utility>

namespace model {

UserStats::UserStats() = default;

UserStats::UserStats(QUuid userId,
                     int messagesSent,
                     int chatsJoined,
                     QStringList exUsernames)
    : m_userId(std::move(userId)),
      m_messagesSent(messagesSent),
      m_chatsJoined(chatsJoined),
      m_exUsernames(std::move(exUsernames))
{
}

const QUuid& UserStats::userId() const
{
    return m_userId;
}

int UserStats::messagesSent() const
{
    return m_messagesSent;
}

int UserStats::chatsJoined() const
{
    return m_chatsJoined;
}

const QStringList& UserStats::exUsernames() const
{
    return m_exUsernames;
}

void UserStats::setUserId(const QUuid& userId)
{
    m_userId = userId;
}

void UserStats::setMessagesSent(int messagesSent)
{
    m_messagesSent = messagesSent;
}

void UserStats::setChatsJoined(int chatsJoined)
{
    m_chatsJoined = chatsJoined;
}

void UserStats::setExUsernames(const QStringList& exUsernames)
{
    m_exUsernames = exUsernames;
}

}
