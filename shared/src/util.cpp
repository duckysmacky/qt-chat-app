#include "util.h"

#include <QDebug>
#include <QRegularExpression>

#include <utility>

namespace shared::util {

QList<Packet> parse(const QByteArray& bytes)
{
    QByteArray buffer = bytes;
    return parseStream(buffer);
}

QList<Packet> parseStream(QByteArray& bytes)
{
    QList<Packet> messages;
    QByteArray buffer;
    bool escaped = false;
    int consumed = 0;

    for (int i = 0; i < bytes.size(); i++)
    {
        const char byte = bytes[i];

        if (escaped)
        {
            if (byte == ESCAPED_DELIMITER)
            {
                buffer.append(DELIMITER);
            }
            else if (byte == ESCAPED_ESCAPE)
            {
                buffer.append(ESCAPE);
            }
            else
            {
                buffer.append(ESCAPE);
                buffer.append(byte);
            }

            escaped = false;
            continue;
        }

        if (byte == ESCAPE)
        {
            escaped = true;
            continue;
        }

        if (byte == DELIMITER)
        {
            if (!buffer.isEmpty())
                messages.append(Packet::deserialize(buffer));

            buffer.clear();
            consumed = i + 1;
            continue;
        }

        buffer.append(byte);
    }

    if (consumed > 0)
        bytes.remove(0, consumed);

    return messages;
}

QByteArray encapsulate(const Packet& packet)
{
    QByteArray payload;
    const QByteArray serialized = packet.serialize();

    for (const char byte : serialized)
    {
        if (byte == DELIMITER)
        {
            payload.append(ESCAPE);
            payload.append(ESCAPED_DELIMITER);
        }
        else if (byte == ESCAPE)
        {
            payload.append(ESCAPE);
            payload.append(ESCAPED_ESCAPE);
        }
        else
        {
            payload.append(byte);
        }
    }

    payload.append(DELIMITER);
    return payload;
}

QString normalizeUsername(QString username)
{
    username = username.trimmed().toLower();

    if (username.startsWith('@'))
        username.remove(0, 1);

    return username;
}

bool isValidUsername(const QString& username)
{
    static const QRegularExpression usernamePattern("^[a-z0-9_]{2,20}$");
    return usernamePattern.match(username).hasMatch();
}

std::optional<Packet> decryptPacketPayload(
    const Packet& packet,
    const QHash<QUuid, KeyStore>& keyStores
)
{
    if (!packet.data().has_value())
        return packet;

    const QUuid& senderSessionId = packet.sender();
    const auto keyStoreIt = keyStores.constFind(senderSessionId);

    if (keyStoreIt == keyStores.constEnd())
    {
        qWarning() << "Cannot decrypt packet: key store not found for session"
                   << senderSessionId.toString();
        return std::nullopt;
    }

    const KeyStore& keyStore = keyStoreIt.value();
    const auto decryptedData = keyStore.decryptForSelf(packet.data().value());

    if (!decryptedData.has_value())
    {
        qWarning() << "Cannot decrypt packet payload from session" << senderSessionId.toString();
        return std::nullopt;
    }

    return Packet(
        packet.type(),
        packet.sender(),
        packet.receiver(),
        std::move(decryptedData.value())
    );
}

} // namespace shared::util
