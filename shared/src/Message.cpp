#include "Message.h"

#include <QDebug>
<<<<<<< HEAD

#include <utility>

/// @brief Size of the serialized message type field (in bytes)
constexpr auto MSG_TYPE_LEN = 1;

/// @brief Size of the serialized sender UUID field (in bytes)
constexpr auto MSG_SENDER_LEN = 16;

namespace shared {

/**
 * @brief Message constructor.
 *
 * Uses move semantics for efficient initialization without copying strings and UUID.
 *
 * @param type Message type.
 * @param sender Sender UUID.
 * @param content Message text content.
 */
Message::Message(const MessageType type, QUuid sender, QString content)
=======
#include <QBuffer>

namespace shared {

static constexpr auto MESSAGE_TYPE_SIZE = 1;
static constexpr auto MESSAGE_HEADER_SIZE = MESSAGE_TYPE_SIZE;

Message::Message(const MessageType type, QString content)
>>>>>>> 7017c698d0b8b90bb82f150f9b42aff160b260a1
    : m_type(type),
      m_content(std::move(content))
{}

/**
 * @brief Move constructor.
 *
 * Moves resources from another Message object.
 *
 * @param other Source object (will be left in a valid but unspecified state).
 */
Message::Message(Message&& other) noexcept
    : m_type(other.m_type),
      m_content(std::move(other.m_content))
{}

<<<<<<< HEAD
/**
 * @brief Move assignment operator.
 *
 * Moves data from another Message object.
 *
 * @param other Source object.
 * @return Reference to this object.
 */
Message& Message::operator =(Message&& other) noexcept
=======
Message& Message::operator=(Message&& other) noexcept
>>>>>>> 7017c698d0b8b90bb82f150f9b42aff160b260a1
{
    if (this == &other) return *this;

    m_type = other.m_type;
    m_content = std::move(other.m_content);

    return *this;
}

<<<<<<< HEAD
/**
 * @brief Decodes a message from its binary representation.
 *
 * Format:
 * [1 byte message type][16 bytes sender UUID][UTF-8 payload]
 *
 * Validates input size and logs a warning if the format is invalid.
 *
 * @param bytes Serialized message data.
 * @return Deserialized Message object.
 */
Message Message::decode(const QByteArray& bytes)
=======
Message Message::deserialize(QByteArray bytes)
>>>>>>> 7017c698d0b8b90bb82f150f9b42aff160b260a1
{
    if (bytes.size() < MESSAGE_HEADER_SIZE)
    {
        qWarning() << "Invalid message payload length:" << bytes.size();
        // we can just ignore this, since message cannot be 0
    }

    QBuffer bytesReader(&bytes);
    bytesReader.open(QIODevice::ReadOnly);

    char messageTypeBuffer[MESSAGE_TYPE_SIZE];
    bytesReader.read(messageTypeBuffer, MESSAGE_TYPE_SIZE);
    const auto type = static_cast<MessageType>(messageTypeBuffer[0]);

    bytes.remove(0, MESSAGE_HEADER_SIZE);
    return Message{type, QString::fromUtf8(bytes)};
}

<<<<<<< HEAD
/**
 * @brief Encodes the message into a binary format.
 *
 * Format:
 * [1 byte message type][16 bytes sender UUID][UTF-8 text]
 *
 * @return QByteArray containing the serialized message.
 */
QByteArray Message::encode() const
=======
QByteArray Message::serialize() const
>>>>>>> 7017c698d0b8b90bb82f150f9b42aff160b260a1
{
    QByteArray bytes;

    bytes.append(static_cast<char>(m_type));
    bytes.append(m_content.toUtf8());

    return bytes;
}

<<<<<<< HEAD
} // namespace shared
=======
} // shared
>>>>>>> 7017c698d0b8b90bb82f150f9b42aff160b260a1
