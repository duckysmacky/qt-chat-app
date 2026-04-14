#include "Chat.h"

#include "Client.h"

Chat::Chat(QObject* parent)
    : QObject(parent)
{
    connect(&Client::instance(), &Client::messageReceived, this, &Chat::onNewMessage);
}

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

void Chat::onNewMessage(const QString& sender, const shared::Message& messagePacket)
{
    if (messagePacket.type() != shared::MessageType::TEXT) return;

    qInfo() << "Incoming text message from" << sender;

    ChatMessage* chatMessage = new ChatMessage(false, messagePacket.content(), sender, this);
    addChatMessage(chatMessage);

    onMessageReceived(chatMessage->id());
}

void Chat::onMessageSent(const QUuid& messageId)
{
    if (ChatMessage* message = findChatMessage(messageId))
        message->markAsSent();
}

void Chat::onMessageDelivered(const QUuid& messageId)
{
    if (ChatMessage* message = findChatMessage(messageId))
        message->setStatus(ChatMessage::Status::Delivered);
}

void Chat::onMessageRead(const QUuid& messageId)
{
    if (ChatMessage* message = findChatMessage(messageId))
        message->setStatus(ChatMessage::Status::Read);
}

void Chat::onMessageReceived(const QUuid& messageId)
{
    if (ChatMessage* message = findChatMessage(messageId))
        message->markAsReceived();
}

void Chat::onMessageDeleted(const QUuid& messageId)
{
    if (ChatMessage* message = findChatMessage(messageId))
        deleteChatMessage(message);
}

void Chat::addChatMessage(ChatMessage* message)
{
    m_messageStorage.insert(message->id(), message);
    m_messageList.append(QVariant::fromValue(message));

    emit messagesChanged();
}

void Chat::deleteChatMessage(ChatMessage* message)
{
    m_messageStorage.remove(message->id());
    m_messageList.removeAll(QVariant::fromValue(message));
    message->deleteLater();

    emit messagesChanged();
}

ChatMessage* Chat::findChatMessage(const QUuid& id) const
{
    return m_messageStorage.value(id, nullptr);
}

std::optional<const ChatMessage*> Chat::findChatMessage(std::function<bool(const ChatMessage*)> predicate) const
{
    for (const QVariant& messageVariant : m_messageList)
    {
        const ChatMessage* message = messageVariant.value<ChatMessage*>();
        if (message != nullptr && predicate(message))
            return message;
    }

    return std::nullopt;
}

std::optional<ChatMessage*> Chat::findChatMessage(std::function<bool(const ChatMessage*)> predicate)
{
    for (const QVariant& messageVariant : m_messageList)
    {
        ChatMessage* message = messageVariant.value<ChatMessage*>();
        if (message != nullptr && predicate(message))
            return message;
    }

    return std::nullopt;
}
