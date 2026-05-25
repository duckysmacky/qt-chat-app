/**
 * @file MessageSender.h
 * @brief Handles sending of chat messages for a specific chat.
 */

#pragma once

#include <QObject>
#include <QByteArray>
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
    QUuid m_chatId;  ///< UUID of the chat associated with this message sender
    QByteArray m_chatMasterKey;

public:
    /**
     * @brief Constructs a MessageSender for a specific chat.
     * @param chatId UUID of the chat that this sender will handle.
     * @param parent Parent QObject (default nullptr).
     */
    explicit MessageSender(QUuid chatId, QObject* parent = nullptr);
    void setChatMasterKey(QByteArray chatMasterKey);

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
};
