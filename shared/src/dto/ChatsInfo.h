/**
 * @file ChatsInfo.h
 * @brief Defines the ChatsInfo class for handling lists of chat information.
 */

#pragma once

#include <QByteArray>
#include <QList>

#include <optional>

#include "ChatInfo.h"

namespace shared {

/**
 * @class ChatsInfo
 * @brief Container for a list of ChatInfo objects.
 * 
 * The ChatsInfo class encapsulates a collection of ChatInfo objects,
 * providing serialization and deserialization functionality for network
 * transmission and storage.
 */
class ChatsInfo
{
public:
    /**
     * @brief Default constructor.
     * 
     * Constructs an empty ChatsInfo object with no chats.
     */
    ChatsInfo();
    
    /**
     * @brief Constructs a ChatsInfo object with a list of chats.
     * @param chats The initial list of ChatInfo objects.
     */
    explicit ChatsInfo(QList<ChatInfo> chats);

    /**
     * @brief Gets the list of chat information objects.
     * @return Constant reference to the internal list of ChatInfo objects.
     */
    const QList<ChatInfo>& chats() const { return m_chats; }

    /**
     * @brief Serializes the ChatsInfo object into a byte array.
     * @return QByteArray containing the serialized data.
     */
    QByteArray serialize() const;
    
    /**
     * @brief Deserializes a byte array into a ChatsInfo object.
     * @param bytes The byte array to deserialize.
     * @return Optional containing the deserialized ChatsInfo if successful,
     *         or std::nullopt if deserialization fails.
     */
    static std::optional<ChatsInfo> deserialize(const QByteArray& bytes);

private:
    QList<ChatInfo> m_chats;  ///< List of chat information objects
};

}