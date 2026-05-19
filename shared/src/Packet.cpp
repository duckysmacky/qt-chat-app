#include "Packet.h"

#include <QBuffer>
#include <QDebug>
#include <QByteArrayView>

#include <utility>

#include "crypto.h"

namespace shared {

static constexpr auto PACKET_TYPE_SIZE = 1;
static constexpr auto UUID_SIZE = 16;
static constexpr auto PACKET_HEADER_SIZE = PACKET_TYPE_SIZE + UUID_SIZE * 2;

Packet::Packet(const PacketType type, QUuid sender, QUuid receiver)
    : m_type(type),
      m_sender(std::move(sender)),
      m_receiver(std::move(receiver))
{
}

Packet::Packet(const PacketType type, QUuid sender, QUuid receiver, QByteArray encryptedPayload)
    : m_type(type),
      m_sender(std::move(sender)),
      m_receiver(std::move(receiver)),
      m_payload(std::move(encryptedPayload))
{
}

Packet::Packet(Packet&& other) noexcept
    : m_type(other.m_type),
      m_sender(std::move(other.m_sender)),
      m_receiver(std::move(other.m_receiver)),
      m_payload(std::move(other.m_payload))
{
}

Packet& Packet::operator =(Packet&& other) noexcept
{
    if (this == &other) return *this;
    m_type = other.m_type;
    m_sender = std::move(other.m_sender);
    m_receiver = std::move(other.m_receiver);
    m_payload = std::move(other.m_payload);
    return *this;
}

void Packet::setPayload(const QByteArray& payload, const QByteArray& encryptionKey)
{
    if (encryptionKey.isEmpty())
    {
        qWarning() << "Cannot encrypt packet payload without an encryption key";
        return;
    }

    m_payload = crypto::encryptBytes(payload, encryptionKey);
}

std::optional<QByteArray> Packet::payload(const QByteArray &decryptionKey) const
{
    if (decryptionKey.isEmpty())
    {
        qWarning() << "Cannot decrypt packet payload without a decryption key";
        return std::nullopt;
    }

    return m_payload.has_value()
        ? std::make_optional(crypto::decryptBytes(m_payload.value(), decryptionKey))
        : std::nullopt;
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
    const auto sender = QUuid::fromRfc4122(QByteArrayView{uuidBuffer, UUID_SIZE});
    bytesReader.read(uuidBuffer, UUID_SIZE);
    const auto receiver = QUuid::fromRfc4122(QByteArrayView{uuidBuffer, UUID_SIZE});

    bytes.remove(0, PACKET_HEADER_SIZE);

    return bytes.isEmpty()
        ? Packet{type, sender, receiver}
        : Packet{type, sender, receiver, std::move(bytes)};
}

QByteArray Packet::serialize() const
{
    QByteArray bytes;

    bytes.append(static_cast<char>(m_type));
    bytes.append(m_sender.toRfc4122());
    bytes.append(m_receiver.toRfc4122());

    if (m_payload.has_value())
        bytes.append(m_payload.value());

    return bytes;
}

}
