#pragma once

#include <QByteArray>
#include <QString>
#include <QUuid>

namespace shared {

enum class MessageType
{
    TEXT,
    MEDIA
};

class Message
{
private:
    QUuid m_senderUserId;
    QUuid m_targetChatId;
    MessageType m_type;
    QString m_content;

public:
    Message(MessageType type, QString content);
    Message(QUuid senderUserId, QUuid targetChatId, MessageType type, QString content);

    Message(const Message& other) = default;
    Message& operator =(const Message& other) = default;
    Message(Message&& other) noexcept;
    Message& operator =(Message&& other) noexcept;

    static Message deserialize(QByteArray bytes);

    QByteArray serialize() const;

    const QUuid& senderUserId() const { return m_senderUserId; }
    const QUuid& targetChatId() const { return m_targetChatId; }
    const MessageType& type() const { return m_type; }
    const QString& content() const { return m_content; }
};

} // shared
