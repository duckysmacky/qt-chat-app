#pragma once

#include <QString>
#include <QByteArray>
#include <QUuid>

namespace shared {

/**
 * @enum MessageType
 * @brief Types of messages exchanged between client and server
 */
enum class MessageType
{
    Text,///<Text message
    Command,///<Command message
    Connect ///<Client connection message
};

/**
 * @class Message
 * @brief Represents a network for exchanging messages between client and server
 * Encapsulates messages type, sender uuid and messages content
 * Provides serialization and deserialization
 */
class Message
{
private:
    MessageType m_type; ///< Message type
    QUuid m_sender;     ///< Sender UUID
    QString m_content;  ///< Message content

public:
    /**
     * @brief Constructs a message.
     * @param type Message type
     * @param sender Sender UUID
     * @param content Message content (optional)
     */
    Message(MessageType type, QUuid sender, QString content = "");

    Message(const Message& other) = default;
    Message& operator =(const Message& other) = default;
    Message(Message&& other) noexcept;
    Message& operator =(Message&& other) noexcept;

    ~Message() = default;
    /**
     * @brief Decodes a message from raw bytes.
     * @param bytes Serialized message data
     * @return Decoded Message object
     */
    static Message decode(const QByteArray& bytes);
    /**
     * @brief Encodes the message into raw bytes.
     * @return Serialized message
     */
    QByteArray encode() const;

    const MessageType& type() const { return m_type; }
    const QUuid& sender() const { return m_sender; }
    const QString& content() const { return m_content; }
};

}
