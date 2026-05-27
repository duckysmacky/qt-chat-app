/**
 * @file MessageSender.h
 * @brief Handles sending of chat messages for a specific chat.
 */

#pragma once

#include <QObject>
#include <QQueue>
#include <QUuid>

#include "ChatMessage.h"

/**
 * @class MessageSender
 * @brief Processes and sends messages for a designated chat.
 * 
 * The MessageSender class is responsible for processing chat messages
 * associated with a specific chat identified by its UUID. It provides
 * a slot to process messages and emits a signal when a message has been
 * successfully sent.
 */
class MessageSender : public QObject
{
    Q_OBJECT

private:
    QUuid m_chatId;         ///< UUID of the chat associated with this message sender
    QUuid m_receiverUserId; ///< User ID of the receiver for direct chats; null for group chats
    mutable QQueue<const ChatMessage*> m_pendingMessages; ///< Messages queued while waiting for receiver's public key

public:
    /**
     * @brief Constructs a MessageSender for a specific chat.
     * @param chatId UUID of the chat that this sender will handle.
     * @param receiverUserId User ID of the other member for direct chats; null UUID for group chats.
     * @param parent Parent QObject (default nullptr).
     */
    explicit MessageSender(QUuid chatId, QUuid receiverUserId, QObject* parent = nullptr);

public slots:
    /**
     * @brief Processes and sends a chat message.
     * @param message Pointer to the ChatMessage to be sent.
     * 
     * This slot is called to process the given message and initiate
     * the sending operation.
     */
    void processMessage(const ChatMessage* message) const;

signals:
    /**
     * @brief Emitted when a message has been successfully sent.
     * @param messageId UUID of the sent message.
     */
    void messageSent(const QUuid& messageId) const;

private slots:
    /**
     * @brief Flushes queued messages when the receiver's public key becomes available.
     * @param peerSessionId Session ID of the peer whose key was just received.
     */
    void onPeerKeyReceived(const QUuid& peerSessionId);
};