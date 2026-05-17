#include "Chat.h"

#include <utility>

namespace model {

Chat::Chat() = default;

QString chatTypeToString(const ChatType type)
{
    switch (type)
    {
    case ChatType::Direct:
        return "direct";
    case ChatType::Group:
        return "group";
    }

    return "direct";
}

ChatType chatTypeFromString(const QString& type)
{
    return type == "group" ? ChatType::Group : ChatType::Direct;
}

Chat::Chat(ChatType type,
           QUuid createdBy)
    : m_id(QUuid::createUuid()),
      m_type(type),
      m_createdBy(std::move(createdBy)),
      m_createdAt(QDateTime::currentDateTime())
{
}

const QUuid& Chat::id() const
{
    return m_id;
}

ChatType Chat::type() const
{
    return m_type;
}

const QUuid& Chat::createdBy() const
{
    return m_createdBy;
}

const QDateTime& Chat::createdAt() const
{
    return m_createdAt;
}

void Chat::setId(const QUuid& id)
{
    m_id = id;
}

void Chat::setType(ChatType type)
{
    m_type = type;
}

void Chat::setCreatedBy(const QUuid& createdBy)
{
    m_createdBy = createdBy;
}

void Chat::setCreatedAt(const QDateTime& createdAt)
{
    m_createdAt = createdAt;
}

}
