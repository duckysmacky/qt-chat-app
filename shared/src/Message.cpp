#include "Message.h"

#include <QDebug>

#include <utility>

constexpr auto MSG_TYPE_LEN = 1;
constexpr auto MSG_SENDER_LEN = 16;

namespace shared {

Message::Message(const MessageType type, QUuid sender, QString content)
    : m_type(type),
      m_sender(std::move(sender)),
      m_content(std::move(content))
{
}

Message Message::decode(QByteArray& bytes)
{
    if (bytes.size() < MSG_TYPE_LEN + MSG_SENDER_LEN)
    {
        qWarning() << "Invalid message payload length:" << bytes.size();
        return Message(MessageType::Command, QUuid(), "");
    }

    const auto type = static_cast<MessageType>(bytes[0]);
    const auto sender = QUuid::fromRfc4122(bytes.mid(MSG_TYPE_LEN, MSG_SENDER_LEN));
    const auto contentBytes = bytes.mid(MSG_TYPE_LEN + MSG_SENDER_LEN);

    if (contentBytes.isEmpty())
        return Message(type, std::move(sender), "");

    const QString content = QString::fromUtf8(contentBytes);

    return Message(type, std::move(sender), std::move(content));
}

QByteArray Message::encode() const
{
    QByteArray result;

    result.append(static_cast<char>(m_type));
    result.append(m_sender.toRfc4122());
    result.append(m_content.toUtf8());

    return result;
}

}
