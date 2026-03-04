#include "middleware.h"

namespace middleware {

QList<shared::Message> parse(const QByteArray& bytes)
{
    QList<shared::Message> messages;
    QByteArray buffer;

    int start = 0;
    for (int i = 0; i < bytes.size(); i++)
    {
        if (bytes[i] == '\x01')
        {
            buffer.append(bytes.mid(start, i - start));
            messages.append(shared::Message::decode(buffer));
            buffer.clear();
            start = i + 1;
        }
    }

    return messages;
}

}