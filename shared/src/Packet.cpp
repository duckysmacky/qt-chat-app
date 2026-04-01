#include "Packet.h"

#include <QDebug>
#include <QBuffer>
#include <QByteArrayView>

#include <utility>

/// @brief Size of the serialized message type field (in bytes)
static constexpr auto PACKET_TYPE_SIZE = 1;
/// @brief Size of the serialized UUID fields (in bytes)
static constexpr auto UUID_SIZE = 16;
/// @brief Size of the packet header (all mandatory fields)
static constexpr auto PACKET_HEADER_SIZE = PACKET_TYPE_SIZE + UUID_SIZE * 2;

namespace shared {

Packet::Packet(const PacketType type, QUuid sender, QUuid target)
    : m_type(type),
      m_sender(std::move(sender)),
      m_target(std::move(target))
{
}

Packet::Packet(const PacketType type, QUuid sender, QUuid target, QByteArray data)
    : m_type(type),
      m_sender(std::move(sender)),
      m_target(std::move(target)),
      m_data(std::move(data))
{
}

Packet::Packet(Packet&& other) noexcept
    : m_type(other.m_type),
      m_sender(std::move(other.m_sender)),
      m_target(std::move(other.m_target)),
      m_data(std::move(other.m_data))
{
}

Packet& Packet::operator =(Packet&& other) noexcept
{
    if (this == &other) return *this;
    m_type = other.m_type;
    m_sender = std::move(other.m_sender);
    m_target = std::move(other.m_target);
    m_data = std::move(other.m_data);
    return *this;
}

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
 * @brief Encodes message using array with bytes
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
