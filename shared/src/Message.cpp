#include "Message.h"

#include <QDebug>
#include <QBuffer>

static constexpr auto MESSAGE_TYPE_SIZE = 1;
static constexpr auto MESSAGE_HEADER_SIZE = MESSAGE_TYPE_SIZE;

namespace shared {

Message::Message(const MessageType type, QString content)
    : m_type(type),
      m_content(std::move(content))
{}

Message::Message(Message&& other) noexcept
    : m_type(other.m_type),
      m_content(std::move(other.m_content))
{}

Message& Message::operator=(Message&& other) noexcept
{
    if (this == &other) return *this;
    m_type = other.m_type;
    m_content = std::move(other.m_content);
    return *this;
}

Message Message::deserialize(QByteArray bytes)
{
    if (bytes.size() < MESSAGE_HEADER_SIZE)
    {
        qWarning() << "Invalid message payload length:" << bytes.size();
        // we can just ignore this, since message cannot be 0
    }

    QBuffer bytesReader(&bytes);
    bytesReader.open(QIODevice::ReadOnly);

    char messageTypeBuffer[MESSAGE_TYPE_SIZE];
    bytesReader.read(messageTypeBuffer, MESSAGE_TYPE_SIZE);
    const auto type = static_cast<MessageType>(messageTypeBuffer[0]);

    bytes.remove(0, MESSAGE_HEADER_SIZE);
    return Message{type, QString::fromUtf8(bytes)};
}

QByteArray Message::serialize() const
{
    QByteArray bytes;

    bytes.append(static_cast<char>(m_type));
    bytes.append(m_content.toUtf8());

    return bytes;
}

} // shared