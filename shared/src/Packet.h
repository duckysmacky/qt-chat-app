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
    /// Invalid packet
    INVALID,
    /// Text message
    MESSAGE,
    /// Special command for server
    COMMAND,
    /// Client connection handshake
    CONNECT,
    // register
    REGISTER,
    // login
    LOGIN,
    // yspeh
    RESULT,
    // vyshel
    LOGOUT,
    PROFILE_REQUEST,
    PROFILE_UPDATE,
    PROFILE_DATA,
    USER_INFO_REQUEST,
    USER_INFO_DATA
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
