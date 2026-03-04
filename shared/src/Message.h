#pragma once

#include <QString>
#include <QByteArray>

namespace shared {

enum class MessageType
{
    Text,
    Command,
};

class Message
{
private:
    MessageType m_type;
    QString m_content;

public:
    Message(MessageType type, QString content);
    ~Message() = default;

    static Message decode(const QByteArray& bytes);

    QByteArray encode() const;

    const MessageType& type() const;
    const QString& content() const;
};

}
