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
    Message(const MessageType type, QString content);
    ~Message() = default;

    static Message decode(QByteArray& bytes);

    QByteArray encode() const;

    const MessageType& type() const;
    const QString& content() const;
};

}
