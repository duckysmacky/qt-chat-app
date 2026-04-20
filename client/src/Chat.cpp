#include "Chat.h"

#include "Client.h"

Chat::Chat(QObject* parent)
    : QObject(parent)
{
    connect(&Client::instance(),
            &Client::messageReceived,
            this,
            &Chat::onNewMessage);
}

/**
 * @brief Sends a message to the server and adds it to local chat history.
 * @param text Message text.
 */
void Chat::submitMessage(const QString& text)
{
    if (text.trimmed().isEmpty()) return;

    Client& client = Client::instance();
    const QString sender = client.uuid().toString();

    ChatMessage* message = new ChatMessage(true, text, sender, this);
    addChatMessage(message);

    qInfo() << "Sending message:" << text;
    client.sendMessage(text);

    onMessageSent(message->id());
}

/**
 * @brief Handles incoming message from server.
 * @param sender Sender UUID or identifier.
 * @param messagePacket Received message packet.
 */
void Chat::onNewMessage(const QString& sender, const shared::Message& messagePacket)
{
    if (messagePacket.type() != shared::MessageType::TEXT) return;

    qInfo() << "Incoming text message from" << sender;

    ChatMessage* chatMessage =
        new ChatMessage(false, messagePacket.content(), sender, this);

    addChatMessage(chatMessage);

    onMessageReceived(chatMessage->id());
}

/**
 * @brief Marks message as successfully sent.
 */
void Chat::onMessageSent(const QUuid& messageId)
{
    if (ChatMessage* message = findChatMessage(messageId))
        message->markAsSent();
}

/**
 * @brief Marks message as delivered.
 */
void Chat::onMessageDelivered(const QUuid& messageId)
{
    if (ChatMessage* message = findChatMessage(messageId))
        message->setStatus(ChatMessage::Status::Delivered);
}

/**
 * @brief Marks message as read.
 */
void Chat::onMessageRead(const QUuid& messageId)
{
    if (ChatMessage* message = findChatMessage(messageId))
        message->setStatus(ChatMessage::Status::Read);
}

/**
 * @brief Marks message as received.
 */
void Chat::onMessageReceived(const QUuid& messageId)
{
    if (ChatMessage* message = findChatMessage(messageId))
        message->markAsReceived();
}

/**
 * @brief Deletes a message from chat.
 */
void Chat::onMessageDeleted(const QUuid& messageId)
{
    if (ChatMessage* message = findChatMessage(messageId))
        deleteChatMessage(message);
}

/**
 * @brief Adds a message to internal storage and UI list.
 * @param message Message to add.
 */
void Chat::addChatMessage(ChatMessage* message)
{
    m_messageStorage.insert(message->id(), message);
    m_messageList.append(QVariant::fromValue(message));

    emit messagesChanged();
}

/**
 * @brief Removes a message from storage and deletes it.
 * @param message Message to delete.
 */
void Chat::deleteChatMessage(ChatMessage* message)
{
    m_messageStorage.remove(message->id());
    m_messageList.removeAll(QVariant::fromValue(message));
    message->deleteLater();

    emit messagesChanged();
}

/**
 * @brief Finds a message by its ID.
 * @param id Message UUID.
 * @return Pointer to message or nullptr.
 */
ChatMessage* Chat::findChatMessage(const QUuid& id) const
{
    return m_messageStorage.value(id, nullptr);
}

/**
 * @brief Finds message using predicate (const version).
 * @param predicate Condition function.
 * @return Optional pointer to message.
 */
std::optional<const ChatMessage*>
Chat::findChatMessage(std::function<bool(const ChatMessage*)> predicate) const
{
    for (const QVariant& messageVariant : m_messageList)
    {
        const ChatMessage* message = messageVariant.value<ChatMessage*>();
        if (message != nullptr && predicate(message))
            return message;
    }

    return std::nullopt;
}

/**
 * @brief Finds message using predicate (mutable version).
 * @param predicate Condition function.
 * @return Optional pointer to message.
 */
std::optional<ChatMessage*>
Chat::findChatMessage(std::function<bool(const ChatMessage*)> predicate)
{
    for (const QVariant& messageVariant : m_messageList)
    {
        ChatMessage* message = messageVariant.value<ChatMessage*>();
        if (message != nullptr && predicate(message))
            return message;
    }

    return std::nullopt;
}