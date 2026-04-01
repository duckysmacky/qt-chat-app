#include "util.h"

namespace shared::util {
/**
 * @brief Splits raw byte stream into messages and decodes them.
 * Iterates through the input data, separates messages using a delimiter
 * (DELIMITER), and decodes each segment into a Message object.
 * @param bytes Raw data received from the network
 * @return List of decoded messages
 * @note Incomplete trailing data (without delimiter) is ignored
 */
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

}
