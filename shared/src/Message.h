#pragma once

#include <QString>
#include <QByteArray>

namespace shared {

enum class MessageType
{
    Text = 0,
    Command = 1
};

class Message
{
private:
    QString m_content;
    MessageType m_type;

public:
    Message(const QString& content, MessageType type);
    ~Message() = default;

    QByteArray encode() const;
    static Message decode(const QByteArray& bytes);

    QString content() const;
    MessageType type() const;
};

}
