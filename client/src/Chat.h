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
 * @brief UI-facing chat controller that manages message lifecycle.
 *
 * Stores sent and received messages and exposes them to the UI layer.
 */
class Chat : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList messages READ messages NOTIFY messagesChanged)

private:
    QHash<QUuid, ChatMessage*> m_messageStorage; ///< Internal message storage by ID
    QVariantList m_messageList;                  ///< UI-facing message list

public:
    /**
     * @brief Constructs Chat controller.
     * @param parent Parent QObject.
     */
    explicit Chat(QObject* parent = nullptr);

    /**
     * @brief Sends a message and adds it to local storage.
     * @param text Message text.
     */
    Q_INVOKABLE void submitMessage(const QString& text);

    /**
     * @brief Returns all messages for UI binding.
     */
    const QVariantList& messages() const { return m_messageList; }

signals:
    /**
     * @brief Emitted when message list changes.
     */
    void messagesChanged();

private slots:
    /**
     * @brief Handles incoming message from network layer.
     */
    void onNewMessage(const QString& sender, const shared::Message& messagePacket);

    /**
     * @brief Called when message is successfully sent.
     */
    void onMessageSent(const QUuid& messageId);

    /**
     * @brief Called when message is delivered to recipient.
     */
    void onMessageDelivered(const QUuid& messageId);

    /**
     * @brief Called when message is marked as read.
     */
    void onMessageRead(const QUuid& messageId);

    /**
     * @brief Called when message is received.
     */
    void onMessageReceived(const QUuid& messageId);

    /**
     * @brief Called when message is deleted.
     */
    void onMessageDeleted(const QUuid& messageId);

private:
    /**
     * @brief Adds a message to internal storage and UI list.
     * @param message Message to add.
     */
    void addChatMessage(ChatMessage* message);

    /**
     * @brief Removes and deletes a message.
     * @param message Message to delete.
     */
    void deleteChatMessage(ChatMessage* message);

    /**
     * @brief Finds a message by its unique ID.
     * @param id Message UUID.
     * @return Pointer to message or nullptr if not found.
     */
    ChatMessage* findChatMessage(const QUuid& id) const;

    /**
     * @brief Finds a message using predicate (const version).
     * @param predicate Condition function.
     * @return Optional pointer to message.
     */
    std::optional<const ChatMessage*>
    findChatMessage(std::function<bool(const ChatMessage*)> predicate) const;

    /**
     * @brief Finds a message using predicate (mutable version).
     * @param predicate Condition function.
     * @return Optional pointer to message.
     */
    std::optional<ChatMessage*>
    findChatMessage(std::function<bool(const ChatMessage*)> predicate);
};