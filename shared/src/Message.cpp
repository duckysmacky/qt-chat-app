#include "Message.h"

namespace shared {

Message::Message(const QString& content, MessageType type): m_content(content), m_type(type) {

}

QByteArray Message::encode() const
{
    QByteArray result;

    result.append(static_cast<char>(m_type));

    result.append(m_content.toUtf8());

    return result;
}

Message Message::decode(const QByteArray& bytes)
{
    if (bytes.isEmpty())
        return Message("", MessageType::Text);

    MessageType type = static_cast<MessageType>(bytes[0]);

    QString content = QString::fromUtf8(bytes.mid(1));

    return Message(content, type);
}

QString Message::content() const
{
    return m_content;
}

MessageType Message::type() const
{
    return m_type;
}

}
