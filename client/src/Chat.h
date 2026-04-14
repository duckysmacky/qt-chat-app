#pragma once

#include <QObject>
#include <QHash>
#include <QVariantList>

#include <functional>
#include <optional>

#include "ChatMessage.h"
#include "Message.h"

/**
 * @class Chat
 * @brief UI-facing class for managing and displaying chat messages.
 * Stores received and sent messages and exposes them
 */
class Chat : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList messages READ messages NOTIFY messagesChanged)

private:
    QHash<QUuid, ChatMessage*> m_messageStorage;
    QVariantList m_messageList;

public:
    explicit Chat(QObject* parent = nullptr);

    Q_INVOKABLE void submitMessage(const QString& text);

    const QVariantList& messages() const { return m_messageList; }

signals:
    void messagesChanged();

private slots:
    void onNewMessage(const QString& sender, const shared::Message& messagePacket);

    void onMessageSent(const QUuid& messageId);
    void onMessageDelivered(const QUuid& messageId);
    void onMessageRead(const QUuid& messageId);
    void onMessageReceived(const QUuid& messageId);
    void onMessageDeleted(const QUuid& messageId);

private:
    void addChatMessage(ChatMessage* message);
    void deleteChatMessage(ChatMessage* message);
    ChatMessage* findChatMessage(const QUuid& id) const;
    std::optional<const ChatMessage*> findChatMessage(std::function<bool(const ChatMessage*)> predicate) const;
    std::optional<ChatMessage*> findChatMessage(std::function<bool(const ChatMessage*)> predicate);
};
