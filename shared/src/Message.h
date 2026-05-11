/**
 * @file Message.h
 * @brief Definition of the Message class and MessageType enum for chat messaging.
 */

#pragma once

#include <QByteArray>
#include <QString>
#include <QUuid>

namespace shared {

/**
 * @enum MessageType
 * @brief Represents the type of a message.
 * 
 * This enumeration distinguishes between different content types
 * that a chat message can contain.
 */
enum class MessageType
{
    TEXT,  ///< Plain text message
    MEDIA  ///< Media message (image, video, audio, etc.)
};

/**
 * @class Message
 * @brief Represents a chat message with type and content.
 * 
 * The Message class encapsulates a chat message including sender information,
 * target chat identification, message type (text or media), and the actual content.
 * It provides serialization and deserialization methods for network transmission.
 */
class Message
{
private:
    QUuid m_senderUserId;      ///< UUID of the user who sent the message.
    QUuid m_targetChatId;      ///< UUID of the chat where the message was sent.
    MessageType m_type;        ///< Type of the message (TEXT or MEDIA).
    QString m_content;         ///< Content of the message (text or media reference).

public:
    /**
     * @brief Constructs a Message with specified type and content.
     * @param type The type of the message (TEXT or MEDIA).
     * @param content The content of the message.
     * 
     * @note This constructor does not initialize senderUserId and targetChatId.
     * They should be set separately or use the full constructor instead.
     */
    Message(MessageType type, QString content);
    
    /**
     * @brief Constructs a complete Message with all fields specified.
     * @param senderUserId UUID of the user sending the message.
     * @param targetChatId UUID of the target chat.
     * @param type The type of the message (TEXT or MEDIA).
     * @param content The content of the message.
     */
    Message(QUuid senderUserId, QUuid targetChatId, MessageType type, QString content);

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
     * 
     * Reconstructs a Message object from its serialized byte representation.
     * The byte array should have been previously created by serialize().
     */
    static Message deserialize(QByteArray bytes);

    /**
     * @brief Serializes the Message object into a byte array.
     * @return QByteArray containing the serialized data.
     * 
     * Converts the Message object into a byte array suitable for network transmission
     * or persistent storage.
     */
    QByteArray serialize() const;

    /**
     * @brief Returns the UUID of the message sender.
     * @return Constant reference to the sender's UUID.
     */
    const QUuid& senderUserId() const { return m_senderUserId; }
    
    /**
     * @brief Returns the UUID of the target chat.
     * @return Constant reference to the target chat's UUID.
     */
    const QUuid& targetChatId() const { return m_targetChatId; }
    
    /**
     * @brief Returns the type of the message.
     * @return Constant reference to the MessageType enum value.
     */
    const MessageType& type() const { return m_type; }

    /**
     * @brief Returns the content of the message.
     * @return Constant reference to the content string.
     */
    const QString& content() const { return m_content; }
};

} // namespace shared