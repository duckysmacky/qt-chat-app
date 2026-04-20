#include "Chat.h"

#include <utility>

namespace model {

Chat::Chat() = default;

Chat::Chat(QString type,
           QUuid createdBy,
           QString title)
    : m_id(QUuid::createUuid()),
      m_type(std::move(type)),
      m_createdBy(std::move(createdBy)),
      m_title(std::move(title)),
      m_createdAt(QDateTime::currentDateTime())
{
}

const QUuid& Chat::id() const
{
    return m_id;
}

const QString& Chat::type() const
{
    return m_type;
}

const QUuid& Chat::createdBy() const
{
    return m_createdBy;
}

const QString& Chat::title() const
{
    return m_title;
}

const QDateTime& Chat::createdAt() const
{
    return m_createdAt;
}

void Chat::setId(const QUuid& id)
{
    m_id = id;
}

void Chat::setType(const QString& type)
{
    m_type = type;
}

void Chat::setCreatedBy(const QUuid& createdBy)
{
    m_createdBy = createdBy;
}

void Chat::setTitle(const QString& title)
{
    m_title = title;
}

void Chat::setCreatedAt(const QDateTime& createdAt)
{
    m_createdAt = createdAt;
}

}
