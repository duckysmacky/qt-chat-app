#pragma once

#include <QObject>
#include <QHash>
#include <QList>
#include <QSet>
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
    Q_PROPERTY(QUuid id READ id CONSTANT)
    Q_PROPERTY(QString label READ label NOTIFY labelChanged)
    Q_PROPERTY(QVariantList messages READ messages NOTIFY messagesChanged)

private:
    const QUuid m_id;
    QSet<QUuid> m_otherMembers;
    QByteArray m_masterKey;
    QList<ChatMessage*> m_pendingOutgoingMessages;
    QList<shared::Message> m_pendingIncomingMessages;
    QSet<QUuid> m_pendingMasterKeyUserIds;
    QHash<QUuid, ChatMessage*> m_messageStorage; ///< Hash map storing chat messages by their UUID
    QVariantList m_messageList;                  ///< List of messages exposed to QML
    QThread m_senderThread;
    MessageSender* m_messageSender;

public:
    /**
     * @brief Constructs a Chat object.
     * @param parent Parent QObject (default nullptr).
     */
    Chat(QUuid id, QSet<QUuid> otherMembers, QObject* parent = nullptr);
    Chat(QUuid id, QSet<QUuid> otherMembers, QByteArray masterKey, QObject* parent = nullptr);
    ~Chat() override;

    /**
     * @brief Submits a new text message to the chat.
     * @param text The text content of the message.
     */
    Q_INVOKABLE void submitMessage(const QString& text);

    /**
     * @brief Gets the label text.
     * @return The label as a QString.
     * 
     * This method is invokable from QML, allowing QML code to retrieve
     * the label text value.
     */
    Q_INVOKABLE QString label() const;
    
    const QUuid& id() const { return m_id; }
    const QSet<QUuid>& otherMembers() const { return m_otherMembers; }
    const QByteArray& masterKey() const { return m_masterKey; }
    bool hasMasterKey() const { return !m_masterKey.isEmpty(); }
    void setMasterKey(QByteArray masterKey);
    void distributeMasterKey();
    void setOtherMembers(QSet<QUuid> otherMembers);
    /// @brief Returns the list of messages for QML consumption.
    /// @return Constant reference to the message list.
    const QVariantList& messages() const { return m_messageList; }

signals:
    void messageSubmitted(ChatMessage* message);

    void labelChanged();
    void messagesChanged(); ///< Emitted when the message list changes.

private slots:
    /**
     * @brief Handles a new incoming message from the network.
     * @param messagePacket The received message packet.
     */
    void onNewMessage(const shared::Message& messagePacket);

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
    void initialize();
    void flushPendingOutgoingMessages();
    void flushPendingIncomingMessages();
    void sendMasterKeyToUser(const QUuid& userId);
    void handleMessage(const shared::Message& messagePacket);

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
