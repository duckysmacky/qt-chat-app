#include "util.h"

namespace shared::util {

QList<Packet> parse(const QByteArray& bytes)
{
    QList<Packet> messages;
    QByteArray buffer;

    int start = 0;
    for (int i = 0; i < bytes.size(); i++)
    {
        if (bytes[i] == DELIMITER)
        {
            buffer.append(bytes.mid(start, i - start));
            if (!buffer.isEmpty())
                messages.append(Packet::deserialize(buffer));
            buffer.clear();
            start = i + 1;
        }
    }

    return messages;
}

QByteArray encapsulate(const Packet& packet)
{
    QByteArray payload;
    payload.append(packet.serialize());
    payload.append(DELIMITER);
    return payload;
}

}
