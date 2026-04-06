#include "ChatMember.h"

#include <utility>

namespace model {

ChatMember::ChatMember() = default;

ChatMember::ChatMember(QUuid chatId,
                       QUuid userId)
    : m_chatId(std::move(chatId)),
      m_userId(std::move(userId))
{
}

const QUuid& ChatMember::chatId() const
{
    return m_chatId;
}

const QUuid& ChatMember::userId() const
{
    return m_userId;
}

void ChatMember::setChatId(const QUuid& chatId)
{
    m_chatId = chatId;
}

void ChatMember::setUserId(const QUuid& userId)
{
    m_userId = userId;
}

}
