#pragma once

#include <QByteArray>
#include <QUuid>

#include <optional>

namespace shared {

/**
 * @enum PacketType
 * @brief Type of packet exchanged between client and server
 */
enum class PacketType
{
    /// Invalid or malformed packet.
    INVALID,
    /// Success or error response.
    OPERATION_RESULT,
    /// Chat message payload.
    CHAT_MESSAGE,
    /// Command sent to the server.
    SERVER_COMMAND,
    /// Client connection handshake.
    CONNECT_CLIENT,
    /// Register a new user account.
    REGISTER_USER,
    /// Log in an existing user.
    LOGIN_USER,
    /// Log out the current user.
    LOGOUT_USER,
    /// Request the authorized user's profile.
    GET_USER_PROFILE,
    /// Update the authorized user's profile.
    UPDATE_USER_PROFILE,
    /// Authorized user's profile data.
    USER_PROFILE_DATA,
    /// Request public information for a user.
    GET_PUBLIC_USER_INFO,
    /// Public user information data.
    PUBLIC_USER_INFO_DATA,
    /// Request available chats.
    GET_CHATS,
    /// Chat list data.
    CHAT_LIST_DATA,
    /// Search chats by query.
    SEARCH_CHATS,
    /// Create a new chat.
    CREATE_CHAT,
    /// Single chat information data.
    CHAT_INFO_DATA
};

/**
 * @class Packet
 * @brief Represents a network for exchanging packets between client and server
 * Encapsulates packet type, sender uuid and packet content
 * Provides serialization and deserialization
 */
class Packet
{
private:
    PacketType m_type;  ///< Packet type
    QUuid m_sender;     ///< Sender UUID
    QUuid m_receiver;   ///< Receiver UUID
    std::optional<QByteArray> m_data;

public:
    Packet(PacketType type, QUuid sender, QUuid receiver);
    Packet(PacketType type, QUuid sender, QUuid receiver, QByteArray data);

    Packet(const Packet& other) = default;
    Packet& operator =(const Packet& other) = default;
    Packet(Packet&& other) noexcept;
    Packet& operator =(Packet&& other) noexcept;

    ~Packet() = default;

    /**
     * @brief Deserializes a message from raw bytes.
     * @param bytes Serialized message data
     * @return Deserialized Message object
     */
    static Packet deserialize(QByteArray bytes);

    /**
     * @brief Serializes the message into raw bytes.
     * @return Serialized message
     */
    QByteArray serialize() const;

    const PacketType& type() const { return m_type; }
    const QUuid& sender() const { return m_sender; }
    const QUuid& receiver() const { return m_receiver; }
    const std::optional<QByteArray>& data() const { return m_data; }
};

}
