#include "Packet.h"

#include <QDebug>
#include <QBuffer>
#include <QByteArrayView>

#include <utility>

namespace shared {

/**
 * @brief Packet header field sizes.
 */
static constexpr auto PACKET_TYPE_SIZE = 1;   ///< Packet type size in bytes
static constexpr auto UUID_SIZE = 16;         ///< UUID size in bytes
static constexpr auto PACKET_HEADER_SIZE = PACKET_TYPE_SIZE + UUID_SIZE * 2; ///< Header size

/**
 * @brief Constructs a packet without payload.
 */
Packet::Packet(const PacketType type, QUuid sender, QUuid target)
    : m_type(type),
      m_sender(std::move(sender)),
      m_target(std::move(target))
{
}

/**
 * @brief Constructs a packet with payload.
 */
Packet::Packet(const PacketType type, QUuid sender, QUuid target, QByteArray data)
    : m_type(type),
      m_sender(std::move(sender)),
      m_target(std::move(target)),
      m_data(std::move(data))
{
}

/**
 * @brief Move constructor.
 */
Packet::Packet(Packet&& other) noexcept
    : m_type(other.m_type),
      m_sender(std::move(other.m_sender)),
      m_target(std::move(other.m_target)),
      m_data(std::move(other.m_data))
{
}

/**
 * @brief Move assignment operator.
 */
Packet& Packet::operator =(Packet&& other) noexcept
{
    if (this == &other) return *this;
    m_type = other.m_type;
    m_sender = std::move(other.m_sender);
    m_target = std::move(other.m_target);
    m_data = std::move(other.m_data);
    return *this;
}

/**
 * @brief Deserializes packet from raw bytes.
 */
Packet Packet::deserialize(QByteArray bytes)
{
    if (bytes.size() < PACKET_HEADER_SIZE)
    {
        qWarning() << "Invalid packet payload length:" << bytes.size();
        return Packet{PacketType::INVALID, QUuid(), QUuid()};
    }

    QBuffer bytesReader(&bytes);
    bytesReader.open(QIODevice::ReadOnly);

    char packetTypeBuffer[PACKET_TYPE_SIZE];
    bytesReader.read(packetTypeBuffer, PACKET_TYPE_SIZE);
    const auto type = static_cast<PacketType>(packetTypeBuffer[0]);

    char uuidBuffer[UUID_SIZE];
    bytesReader.read(uuidBuffer, UUID_SIZE);
    const auto sender = QUuid::fromBytes(uuidBuffer);
    bytesReader.read(uuidBuffer, UUID_SIZE);
    const auto target = QUuid::fromBytes(uuidBuffer);

    bytes.remove(0, PACKET_HEADER_SIZE);

    return bytes.isEmpty()
        ? Packet{type, sender, target}
        : Packet{type, sender, target, std::move(bytes)};
}

/**
 * @brief Serializes packet into raw bytes.
 */
QByteArray Packet::serialize() const
{
    QByteArray bytes;

    bytes.append(static_cast<char>(m_type));
    bytes.append(m_sender.toRfc4122());
    bytes.append(m_target.toRfc4122());

    if (m_data.has_value())
        bytes.append(m_data.value());

    return bytes;
}

}