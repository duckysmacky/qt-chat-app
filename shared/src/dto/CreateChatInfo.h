/**
 * @file CreateChatInfo.h
 * @brief Definition of the ChatCreateInfo DTO class for chat creation requests.
 */

#pragma once

#include <QByteArray>
#include <QList>
#include <QUuid>

#include <optional>

namespace shared {

/**
 * @class ChatCreateInfo
 * @brief Data transfer object containing information required to create a new chat.
 * 
 * The ChatCreateInfo class encapsulates the necessary data for creating a new chat,
 * primarily the list of member user IDs that should be included in the chat.
 * This DTO is sent from client to server when requesting the creation of a new chat.
 */
class ChatCreateInfo
{
public:
    /**
     * @brief Constructs a ChatCreateInfo object with the specified member IDs.
     * @param memberIds List of UUIDs for users to be added as members of the new chat.
     * 
     * Creates a chat creation request with the given initial members.
     * The chat type (private/group) is typically inferred from the number of members.
     */
    explicit ChatCreateInfo(QList<QUuid> memberIds);

    /**
     * @brief Returns the list of member user IDs for the new chat.
     * @return Constant reference to the list of member UUIDs.
     */
    const QList<QUuid>& memberIds() const { return m_memberIds; }

    /**
     * @brief Serializes the ChatCreateInfo object into a byte array.
     * @return QByteArray containing the serialized data.
     * 
     * Converts the ChatCreateInfo object into a byte array suitable for network transmission.
     * The serialized format includes the list of member IDs to be added to the chat.
     */
    QByteArray serialize() const;
    
    /**
     * @brief Deserializes a byte array into a ChatCreateInfo object.
     * @param bytes The byte array to deserialize.
     * @return std::optional<ChatCreateInfo> containing the deserialized object if successful,
     *         or std::nullopt if deserialization fails.
     * 
     * Reconstructs a ChatCreateInfo object from its serialized byte representation.
     * Returns an empty optional if the byte array is invalid or corrupted.
     */
    static std::optional<ChatCreateInfo> deserialize(const QByteArray& bytes);

private:
    QList<QUuid> m_memberIds;   ///< List of user UUIDs to be included as members in the new chat.
};

} // namespace shared