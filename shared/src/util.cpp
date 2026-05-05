#include "util.h"

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

}
