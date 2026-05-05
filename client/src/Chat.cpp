#include "Chat.h"

#include <utility>

#include "AccountManager.h"
#include "RequestManager.h"
#include "UserResolver.h"

Chat::Chat(QUuid id, QSet<QUuid> otherMembers, QObject* parent)
    : QObject(parent),
      m_id(std::move(id)),
      m_otherMembers(std::move(otherMembers)),
      m_messageSender(new MessageSender(m_id))
{
    m_messageSender->moveToThread(&m_senderThread);

    connect(&m_senderThread, &QThread::finished, m_messageSender, &QObject::deleteLater);
    connect(this, &Chat::messageSubmitted, m_messageSender, &MessageSender::processMessage);
    connect(m_messageSender, &MessageSender::messageSent, this, &Chat::onMessageSent);

    connect(&RequestManager::instance(), &RequestManager::chatMessageReceived, this, &Chat::onNewMessage);
    connect(&UserResolver::instance(), &UserResolver::userResolved, this, [this](const QUuid& userId, const shared::PublicUserInfo&) {
        if (m_otherMembers.contains(userId))
            emit labelChanged();
    });

    for (const QUuid& userId : m_otherMembers)
        UserResolver::instance().resolveUser(userId);

    m_senderThread.start();
}

Chat::~Chat()
{
    m_senderThread.quit();
    m_senderThread.wait();
}

void Chat::submitMessage(const QString& text)
{
    if (text.trimmed().isEmpty()) return;

    const QUuid senderUserId = AccountManager::instance().userId().value_or(QUuid());

    const auto message = new ChatMessage(true, text, senderUserId, this);
    addChatMessage(message);

    emit messageSubmitted(message);
}

QString Chat::label() const
{
    QString label;

    for (const QUuid& userId : m_otherMembers)
    {
        QString username = "Unknown";

        if (!userId.isNull()) {
            const auto userInfo = UserResolver::instance().resolveUser(userId);
            username = userInfo.has_value() ? userInfo->username() : "Loading...";
        }

        if (!label.isEmpty()) {
            label.append(", ");
        }

        label.append(username);
    }

    return label;
}

void Chat::onNewMessage(const shared::Message& messagePacket)
{
    if (messagePacket.targetChatId() != m_id) return;
    if (messagePacket.type() != shared::MessageType::TEXT) return;

    const QUuid& senderUserId = messagePacket.senderUserId();
    qInfo() << "Incoming text message from" << senderUserId;

    ChatMessage* chatMessage = new ChatMessage(false, messagePacket.content(), senderUserId, this);
    addChatMessage(chatMessage);

    onMessageReceived(chatMessage->id());
}

void Chat::onMessageSent(const QUuid& messageId) const
{
    if (ChatMessage* message = findChatMessage(messageId))
        message->markAsSent();
}

void Chat::onMessageDelivered(const QUuid& messageId) const
{
    if (ChatMessage* message = findChatMessage(messageId))
        message->setStatus(ChatMessage::Status::Delivered);
}

void Chat::onMessageRead(const QUuid& messageId) const
{
    if (ChatMessage* message = findChatMessage(messageId))
        message->setStatus(ChatMessage::Status::Read);
}

void Chat::onMessageReceived(const QUuid& messageId) const
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
