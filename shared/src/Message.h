#pragma once

#include <QString>

namespace shared {

enum class MessageType
{
    TEXT,
    MEDIA
};

class Message
{
private:
    MessageType m_type;
    QString m_content;

public:
    Message(MessageType type, QString content);

    Message(const Message& other) = default;
    Message& operator =(const Message& other) = default;
    Message(Message&& other) noexcept;
    Message& operator =(Message&& other) noexcept;

    static Message deserialize(QByteArray bytes);

    QByteArray serialize() const;

    const MessageType& type() const { return m_type; }
    const QString& content() const { return m_content; }
};

} // shared