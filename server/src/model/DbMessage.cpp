#include "DbMessage.h"

#include <utility>

namespace model {

DbMessage::DbMessage() = default;

DbMessage::DbMessage(QUuid toId,
                     QUuid fromId,
                     QUuid contentId,
                     QUuid chatId)
    : m_id(QUuid::createUuid()),
      m_toId(std::move(toId)),
      m_fromId(std::move(fromId)),
      m_contentId(std::move(contentId)),
      m_chatId(std::move(chatId)),
      m_createdAt(QDateTime::currentDateTime())
{
}

const QUuid& DbMessage::id() const
{
    return m_id;
}

const QUuid& DbMessage::toId() const
{
    return m_toId;
}

const QUuid& DbMessage::fromId() const
{
    return m_fromId;
}

const QUuid& DbMessage::contentId() const
{
    return m_contentId;
}

const QUuid& DbMessage::chatId() const
{
    return m_chatId;
}

const QDateTime& DbMessage::createdAt() const
{
    return m_createdAt;
}

const QDateTime& DbMessage::updatedAt() const
{
    return m_updatedAt;
}

void DbMessage::setId(const QUuid& id)
{
    m_id = id;
}

void DbMessage::setToId(const QUuid& toId)
{
    m_toId = toId;
}

void DbMessage::setFromId(const QUuid& fromId)
{
    m_fromId = fromId;
}

void DbMessage::setContentId(const QUuid& contentId)
{
    m_contentId = contentId;
}

void DbMessage::setChatId(const QUuid& chatId)
{
    m_chatId = chatId;
}

void DbMessage::setCreatedAt(const QDateTime& createdAt)
{
    m_createdAt = createdAt;
}

void DbMessage::setUpdatedAt(const QDateTime& updatedAt)
{
    m_updatedAt = updatedAt;
}

}
