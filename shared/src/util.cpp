#include "util.h"

namespace shared::util {

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
