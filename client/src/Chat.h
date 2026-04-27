#pragma once

#include <QObject>
#include <QHash>
#include <QVariantList>
#include <QThread>

#include <functional>
#include <optional>

#include "ChatMessage.h"
#include "Message.h"
#include "MessageSender.h"

/**
 * @class Chat
 * @brief Manages a chat session, including message storage and UI interaction.
 *        Exposes messages as a QVariantList for QML consumption.
 */
class Chat : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList messages READ messages NOTIFY messagesChanged)

private:
    QHash<QUuid, ChatMessage*> m_messageStorage; ///< Hash map storing chat messages by their UUID
    QVariantList m_messageList;                  ///< List of messages exposed to QML
    QThread m_senderThread;
    MessageSender* m_messageSender;

public:
    /**
     * @brief Constructs a Chat object.
     * @param parent Parent QObject (default nullptr).
     */
    explicit Chat(QObject* parent = nullptr);
    ~Chat() override;

    /**
     * @brief Submits a new text message to the chat.
     * @param text The text content of the message.
     */
    Q_INVOKABLE void submitMessage(const QString& text);

    /// @brief Returns the list of messages for QML consumption.
    /// @return Constant reference to the message list.
    const QVariantList& messages() const { return m_messageList; }

signals:
    void messageSubmitted(ChatMessage* message);
    void messagesChanged(); ///< Emitted when the message list changes.

private slots:
    /**
     * @brief Handles a new incoming message from the network.
     * @param sender The username of the message sender.
     * @param messagePacket The received message packet.
     */
    void onNewMessage(const QString& sender, const shared::Message& messagePacket);

    /**
     * @brief Handles the event when a message is sent.
     * @param messageId The UUID of the sent message.
     */
    void onMessageSent(const QUuid& messageId) const;

    /**
     * @brief Handles the event when a message is delivered.
     * @param messageId The UUID of the delivered message.
     */
    void onMessageDelivered(const QUuid& messageId) const;

    /**
     * @brief Handles the event when a message is read.
     * @param messageId The UUID of the read message.
     */
    void onMessageRead(const QUuid& messageId) const;

    /**
     * @brief Handles the event when a message is received.
     * @param messageId The UUID of the received message.
     */
    void onMessageReceived(const QUuid& messageId) const;

    /**
     * @brief Handles the event when a message is deleted.
     * @param messageId The UUID of the deleted message.
     */
    void onMessageDeleted(const QUuid& messageId);

private:
    /**
     * @brief Adds a chat message to storage and updates the QML list.
     * @param message The ChatMessage to add.
     */
    void addChatMessage(ChatMessage* message);

    /**
     * @brief Deletes a chat message from storage and updates the QML list.
     * @param message The ChatMessage to delete.
     */
    void deleteChatMessage(ChatMessage* message);

    /**
     * @brief Finds a chat message by its UUID.
     * @param id The UUID of the message to find.
     * @return Pointer to the ChatMessage if found, nullptr otherwise.
     */
    ChatMessage* findChatMessage(const QUuid& id) const;

    /**
     * @brief Finds a chat message using a predicate (const version).
     * @param predicate A function that takes a ChatMessage pointer and returns bool.
     * @return An optional containing the const ChatMessage pointer if found.
     */
    std::optional<const ChatMessage*> findChatMessage(std::function<bool(const ChatMessage*)> predicate) const;

    /**
     * @brief Finds a chat message using a predicate (non-const version).
     * @param predicate A function that takes a ChatMessage pointer and returns bool.
     * @return An optional containing the ChatMessage pointer if found.
     */
    std::optional<ChatMessage*> findChatMessage(std::function<bool(const ChatMessage*)> predicate);
};