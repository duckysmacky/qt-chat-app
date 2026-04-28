#include "Message.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#include <utility>

namespace shared {

Message::Message(MessageType type, QString content)
    : Message(QUuid(), QUuid(), type, std::move(content))
{
}

Message::Message(QUuid senderUserId, QUuid targetChatId, MessageType type, QString content)
    : m_senderUserId(std::move(senderUserId)),
      m_targetChatId(std::move(targetChatId)),
      m_type(type),
      m_content(std::move(content))
{
}

Message::Message(Message&& other) noexcept
    : m_senderUserId(std::move(other.m_senderUserId)),
      m_targetChatId(std::move(other.m_targetChatId)),
      m_type(other.m_type),
      m_content(std::move(other.m_content))
{
}

Message& Message::operator=(Message&& other) noexcept
{
    if (this == &other)
        return *this;

    m_senderUserId = std::move(other.m_senderUserId);
    m_targetChatId = std::move(other.m_targetChatId);
    m_type = other.m_type;
    m_content = std::move(other.m_content);

    return *this;
}

Message Message::deserialize(QByteArray bytes)
{
    const QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (doc.isObject())
    {
        const QJsonObject obj = doc.object();

        if (obj.contains("senderUserId") &&
            obj.contains("targetChatId") &&
            obj.contains("type") &&
            obj.contains("content"))
        {
            return Message(
                QUuid(obj["senderUserId"].toString()),
                QUuid(obj["targetChatId"].toString()),
                static_cast<MessageType>(obj["type"].toInt()),
                obj["content"].toString()
            );
        }
    }

    if (bytes.isEmpty())
    {
        qWarning() << "Invalid empty message payload";
        return Message(MessageType::TEXT, "");
    }

    const auto type = static_cast<MessageType>(bytes[0]);
    bytes.remove(0, 1);

    return Message(QUuid(), QUuid(), type, QString::fromUtf8(bytes));
}

QByteArray Message::serialize() const
{
    QJsonObject obj;
    obj["senderUserId"] = m_senderUserId.toString(QUuid::WithoutBraces);
    obj["targetChatId"] = m_targetChatId.toString(QUuid::WithoutBraces);
    obj["type"] = static_cast<int>(m_type);
    obj["content"] = m_content;

    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

} // shared
