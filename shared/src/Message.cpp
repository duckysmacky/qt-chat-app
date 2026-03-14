#include "Message.h"

#include <QBuffer>

#include <utility>

constexpr auto MSG_TYPE_LEN = 1;

namespace shared {

Message::Message(const MessageType type, QString content)
    : m_type(type),
      m_content(std::move(content))
{
}

Message Message::decode(QByteArray& bytes)
{
    QBuffer buffer(&bytes);

    const auto typeBytes = buffer.read(MSG_TYPE_LEN);
    const auto type = static_cast<MessageType>(typeBytes[0]);

    const auto contentBytes = buffer.readAll();
    if (contentBytes.isEmpty())
        return Message(type, "");

    const QString content = QString::fromUtf8(contentBytes);

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
