#include "Message.h"

#include <utility>

namespace shared {

Message::Message(const MessageType type, QString content)
    : m_type(type),
      m_content(std::move(content))
{
}

Message Message::decode(const QByteArray& bytes)
{
    if (bytes.isEmpty())
        return Message(MessageType::Text, "");

    const auto type = static_cast<MessageType>(bytes[0]);
    const QString content = QString::fromUtf8(bytes.mid(1));

    return Message(type, content);
}

QByteArray Message::encode() const
{
    QByteArray result;

    result.append(static_cast<char>(m_type));
    result.append(m_content.toUtf8());

    return result;
}

const MessageType& Message::type() const
{
    return m_type;
}

const QString& Message::content() const
{
    return m_content;
}

}
