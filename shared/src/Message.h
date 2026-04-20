#pragma once

#include <QString>

namespace shared {

/**
 * @brief Message type enum.
 */
enum class MessageType
{
    TEXT,
    MEDIA
};

/**
 * @brief Simple message model with serialization support.
 */
class Message
{
private:
    MessageType m_type;
    QString m_content;

public:
    /**
     * @brief Constructs a message.
     */
    Message(MessageType type, QString content);

    Message(const Message& other) = default;
    Message& operator=(const Message& other) = default;
    Message(Message&& other) noexcept;
    Message& operator=(Message&& other) noexcept;

    /**
     * @brief Deserializes message from bytes.
     */
    static Message deserialize(QByteArray bytes);

    /**
     * @brief Serializes message to bytes.
     */
    QByteArray serialize() const;

    /// Message type.
    const MessageType& type() const { return m_type; }

    /// Message content.
    const QString& content() const { return m_content; }
};

} // shared