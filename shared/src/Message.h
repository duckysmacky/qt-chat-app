#pragma once

#include <QString>
#include <QByteArray>
#include <QUuid>

namespace shared {

enum class MessageType
{
    Text,
    Command,
    Connect
};

class Message
{
private:
    MessageType m_type;
    QUuid m_sender;
    QString m_content;

public:
    Message(MessageType type, QUuid sender, QString content = "");
    ~Message() = default;

    static Message decode(QByteArray& bytes);

    QByteArray encode() const;

    const MessageType& type() const { return m_type; }
    const QUuid& sender() const { return m_sender; }
    const QString& content() const { return m_content; }
};

}
