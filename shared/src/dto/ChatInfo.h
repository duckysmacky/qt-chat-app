/**
 * @file ChatInfo.h
 * @brief Definition of the ChatInfo DTO class for chat metadata.
 */

#pragma once

#include <QByteArray>
#include <QDateTime>
#include <QList>
#include <QString>
#include <QUuid>

#include <optional>

namespace shared {

/**
 * @class ChatInfo
 * @brief Data transfer object containing detailed information about a chat.
 * 
 * The ChatInfo class encapsulates complete metadata for a chat, including its
 * unique identifier, type (e.g., "private", "group"), creator information,
 * creation timestamp, and the list of member user IDs. This DTO is used for
 * transmitting chat details between client and server.
 */
class ChatInfo
{
public:
    /**
     * @brief Default constructor.
     * 
     * Creates an empty ChatInfo object with uninitialized fields.
     */
    ChatInfo();
    
    /**
     * @brief Constructs a ChatInfo with basic chat information (no members).
     * @param id The unique identifier of the chat.
     * @param type The type of chat (e.g., "private", "group").
     * @param createdBy UUID of the user who created the chat.
     * @param createdAt Timestamp when the chat was created.
     */
    ChatInfo(QUuid id, QString type, QUuid createdBy, QDateTime createdAt);
    
    /**
     * @brief Constructs a complete ChatInfo with all fields including members.
     * @param id The unique identifier of the chat.
     * @param type The type of chat (e.g., "private", "group").
     * @param createdBy UUID of the user who created the chat.
     * @param createdAt Timestamp when the chat was created.
     * @param memberIds List of UUIDs for all members in the chat.
     */
    ChatInfo(QUuid id, QString type, QUuid createdBy, QDateTime createdAt, QList<QUuid> memberIds);

    /**
     * @brief Returns the unique identifier of the chat.
     * @return Constant reference to the chat's UUID.
     */
    const QUuid& id() const { return m_id; }
    
    /**
     * @brief Returns the type of the chat.
     * @return Constant reference to the chat type string.
     */
    const QString& type() const { return m_type; }
    
    /**
     * @brief Returns the UUID of the user who created the chat.
     * @return Constant reference to the creator's UUID.
     */
    const QUuid& createdBy() const { return m_createdBy; }
    
    /**
     * @brief Returns the creation timestamp of the chat.
     * @return Constant reference to the creation QDateTime.
     */
    const QDateTime& createdAt() const { return m_createdAt; }
    
    /**
     * @brief Returns the list of member user IDs in the chat.
     * @return Constant reference to the list of member UUIDs.
     */
    const QList<QUuid>& memberIds() const { return m_memberIds; }

    /**
     * @brief Serializes the ChatInfo object into a byte array.
     * @return QByteArray containing the serialized data.
     * 
     * Converts the ChatInfo object into a byte array suitable for network transmission
     * or persistent storage. The serialized format includes all chat metadata fields.
     */
    QByteArray serialize() const;
    
    /**
     * @brief Deserializes a byte array into a ChatInfo object.
     * @param bytes The byte array to deserialize.
     * @return std::optional<ChatInfo> containing the deserialized object if successful,
     *         or std::nullopt if deserialization fails.
     * 
     * Reconstructs a ChatInfo object from its serialized byte representation.
     * Returns an empty optional if the byte array is invalid or corrupted.
     */
    static std::optional<ChatInfo> deserialize(const QByteArray& bytes);

private:
    QUuid m_id;                           ///< Unique identifier of the chat.
    QString m_type;                       ///< Type of chat (e.g., "private", "group").
    QUuid m_createdBy;                    ///< UUID of the user who created the chat.
    QDateTime m_createdAt;                ///< Timestamp of chat creation.
    QList<QUuid> m_memberIds;             ///< List of member user IDs in the chat.
};

} // namespace shared