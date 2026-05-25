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
    INVALID, ///< Invalid or malformed message
    TEXT,    ///< Plain text message
    MEDIA    ///< Media message (image, video, audio, etc.)
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
    QByteArray m_content;      ///< Encrypted message content bytes.

public:
    /**
     * @brief Constructs a Message with sender, target chat, and type.
     * @param senderUserId UUID of the user sending the message.
     * @param targetChatId UUID of the target chat.
     * @param type The type of the message (TEXT or MEDIA).
     *
     * @note Content must be set with setContent().
     */
    Message(QUuid senderUserId, QUuid targetChatId, MessageType type);

    Message(const Message& other) = default;
    Message& operator =(const Message& other) = default;
    Message(Message&& other) noexcept;
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
     * @brief Encrypts and stores the message content.
     * @param content Plain content string.
     * @param encryptionKey Chat master key used to encrypt content.
     */
    void setContent(const QString& content, const QByteArray& encryptionKey);

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
     * @brief Decrypts and returns the message content.
     * @param decryptionKey Chat master key used to decrypt content.
     * @return Decrypted content string.
     */
    QString content(const QByteArray& decryptionKey) const;

private:
    Message(QUuid senderUserId, QUuid targetChatId, MessageType type, QByteArray contentBytes);
};

} // namespace shared
