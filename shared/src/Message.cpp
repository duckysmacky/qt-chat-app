#include "Message.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#include <utility>

#include "crypto.h"

namespace shared {

namespace {

constexpr auto senderUserIdKey = "senderUserId";
constexpr auto targetChatIdKey = "targetChatId";
constexpr auto typeKey = "type";
constexpr auto contentKey = "content";

} // namespace

Message::Message(QUuid senderUserId, QUuid targetChatId, MessageType type)
    : m_senderUserId(std::move(senderUserId)),
      m_targetChatId(std::move(targetChatId)),
      m_type(type)
{
}

Message::Message(QUuid senderUserId, QUuid targetChatId, MessageType type, QByteArray contentBytes)
    : m_senderUserId(std::move(senderUserId)),
      m_targetChatId(std::move(targetChatId)),
      m_type(type),
      m_content(std::move(contentBytes))
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

        if (obj.contains(senderUserIdKey) &&
            obj.contains(targetChatIdKey) &&
            obj.contains(typeKey) &&
            obj.contains(contentKey))
        {
            return Message(
                QUuid(obj[senderUserIdKey].toString()),
                QUuid(obj[targetChatIdKey].toString()),
                static_cast<MessageType>(obj[typeKey].toInt()),
                QByteArray::fromBase64(obj[contentKey].toString().toLatin1())
            );
        }
    }

    if (bytes.isEmpty())
    {
        qWarning() << "Invalid empty message payload";
        return Message{QUuid(), QUuid(), MessageType::INVALID};
    }

    const auto type = static_cast<MessageType>(bytes[0]);
    bytes.remove(0, 1);

    return Message(QUuid(), QUuid(), type, std::move(bytes));
}

QByteArray Message::serialize() const
{
    QJsonObject obj;
    obj[senderUserIdKey] = m_senderUserId.toString(QUuid::WithoutBraces);
    obj[targetChatIdKey] = m_targetChatId.toString(QUuid::WithoutBraces);
    obj[typeKey] = static_cast<int>(m_type);
    obj[contentKey] = QString::fromLatin1(m_content.toBase64());

    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

void Message::setContent(const QString& content, const QByteArray& encryptionKey)
{
    m_content = crypto::encryptHybrid(content.toUtf8(), encryptionKey);
}

QString Message::content(const QByteArray& decryptionKey) const
{
    return QString::fromUtf8(crypto::decryptHybrid(m_content, decryptionKey));
}

} // shared
