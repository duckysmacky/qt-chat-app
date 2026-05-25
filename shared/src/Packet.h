/**
 * @file Packet.h
 * @brief Defines the Packet class and PacketType enum for network communication.
 */

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
    /// Connection handshake.
    CONNECT,
    /// Success or error response.
    OPERATION_RESULT,
    /// Chat message payload.
    CHAT_MESSAGE,
    /// Command sent to the server.
    SERVER_COMMAND,
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
    GET_USER_INFO,
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
    CHAT_INFO_DATA,
    /// Public key exchange packet.
    PUBLIC_KEY_EXCHANGE,
    /// Chat master key transfer packet.
    CHAT_KEY_EXCHANGE,
    /// Request the current session id for a user id.
    GET_USER_SESSION,
    /// Current session id for a user id.
    USER_SESSION_DATA,
    /// Store encrypted chat master key backup for the authorized user.
    STORE_CHAT_KEY,
    /// Request encrypted chat master key backups for the authorized user.
    GET_CHAT_KEYS,
    /// Encrypted chat master key backup data.
    CHAT_KEYS_DATA
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
    std::optional<QByteArray> m_payload;  ///< Optional packet data payload

public:
    /**
     * @brief Constructs a Packet with type, sender, and receiver.
     * @param type The type of the packet.
     * @param sender UUID of the sender.
     * @param receiver UUID of the receiver.
     */
    Packet(PacketType type, QUuid sender, QUuid receiver);

    /**
     * @brief Copy constructor.
     * @param other The Packet to copy from.
     */
    Packet(const Packet& other) = default;
    
    /**
     * @brief Copy assignment operator.
     * @param other The Packet to copy from.
     * @return Reference to this Packet.
     */
    Packet& operator =(const Packet& other) = default;
    
    /**
     * @brief Move constructor.
     * @param other The Packet to move from.
     */
    Packet(Packet&& other) noexcept;
    
    /**
     * @brief Move assignment operator.
     * @param other The Packet to move from.
     * @return Reference to this Packet.
     */
    Packet& operator =(Packet&& other) noexcept;

    /**
     * @brief Destructor.
     */
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

    void setPayload(const QByteArray& payload, const QByteArray& encryptionKey);

    bool hasPayload() const { return m_payload.has_value(); };

    /**
     * @brief Gets the packet type.
     * @return Constant reference to the packet type.
     */
    const PacketType& type() const { return m_type; }
    
    /**
     * @brief Gets the sender UUID.
     * @return Constant reference to the sender UUID.
     */
    const QUuid& sender() const { return m_sender; }
    
    /**
     * @brief Gets the receiver UUID.
     * @return Constant reference to the receiver UUID.
     */
    const QUuid& receiver() const { return m_receiver; }
    
    /**
     * @brief Gets the packet data payload.
     * @return Constant optional reference to the packet data.
     */
    std::optional<QByteArray> payload(const QByteArray& decryptionKey) const;

private:
    Packet(PacketType type, QUuid sender, QUuid receiver, QByteArray encryptedPayload);
};

}
