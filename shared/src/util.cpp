#include "util.h"

namespace shared::util {
/**
 * @brief Splits raw byte stream into messages and decodes them.
 * Iterates through the input data, separates messages using a delimiter
 * ('\x01'), and decodes each segment into a Message object.
 * @param bytes Raw data received from the network
 * @return List of decoded messages
 * @note Incomplete trailing data (without delimiter) is ignored
 */
QList<Message> parse(const QByteArray& bytes)
{
    QList<Message> messages;
    QByteArray buffer;

    int start = 0;
    for (int i = 0; i < bytes.size(); i++)
    {
        if (bytes[i] == '\x01')
        {
            buffer.append(bytes.mid(start, i - start));
            if (!buffer.isEmpty())
                messages.append(Message::decode(buffer));
            buffer.clear();
            start = i + 1;
        }
    }

    return messages;
}

}
