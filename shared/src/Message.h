#pragma once

#include <QString>
#include <QByteArray>

namespace shared {

/**
 * @enum MessageType
 * @brief Represents the type of a message.
 */
enum class MessageType
{
    TEXT,  ///< Plain text message
    MEDIA  ///< Media message (image, video, audio, etc.)
};

/**
 * @class Message
 * @brief Represents a chat message with type and content.
 */
class Message
{
private:
    MessageType m_type;    ///< Type of the message (text or media)
    QString m_content;     ///< Content of the message

public:
    /**
     * @brief Constructs a Message with specified type and content.
     * @param type The type of the message.
     * @param content The content of the message.
     */
    Message(MessageType type, QString content);

    /// @brief Default copy constructor.
    Message(const Message& other) = default;

    /// @brief Default copy assignment operator.
    Message& operator =(const Message& other) = default;

    /// @brief Move constructor.
    /// @param other The Message object to move from.
    Message(Message&& other) noexcept;

    /// @brief Move assignment operator.
    /// @param other The Message object to move from.
    /// @return Reference to this Message.
    Message& operator =(Message&& other) noexcept;

    /**
     * @brief Deserializes a byte array into a Message object.
     * @param bytes The byte array to deserialize.
     * @return The deserialized Message object.
     */
    static Message deserialize(QByteArray bytes);

    /**
     * @brief Serializes the Message object into a byte array.
     * @return QByteArray containing the serialized data.
     */
    QByteArray serialize() const;

    /// @brief Returns the type of the message.
    /// @return Constant reference to the MessageType.
    const MessageType& type() const { return m_type; }

    /// @brief Returns the content of the message.
    /// @return Constant reference to the content string.
    const QString& content() const { return m_content; }
};

} // namespace shared