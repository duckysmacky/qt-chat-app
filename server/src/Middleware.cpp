#include "Middleware.h"

namespace middleware {

QList<QByteArray> parse(const QByteArray& bytes) {
    QList<QByteArray> messages;
    QByteArray buffer;

    int start = 0;
    for (int i = 0; i < bytes.size(); i++) {
        if (bytes[i] == '\x01') {
            buffer.append(bytes.mid(start, i - start));
            messages.append(buffer);
            buffer.clear();
            start = i + 1;
        }
    }

    return messages;
}
}
