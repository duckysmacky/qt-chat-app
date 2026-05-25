#include "Chat.h"

#include <QMetaObject>

#include <utility>

#include "AccountManager.h"
#include "KeyStore.h"
#include "RequestManager.h"
#include "SessionResolver.h"
#include "UserResolver.h"
#include "crypto.h"

Chat::Chat(QUuid id, QSet<QUuid> otherMembers, QObject* parent)
    : QObject(parent),
      m_id(std::move(id)),
      m_otherMembers(std::move(otherMembers)),
      m_masterKeyRequestStarted(false),
      m_masterKeyRequestSent(false),
      m_messageSender(new MessageSender(m_id))
{
    initialize();
}

Chat::Chat(QUuid id, QSet<QUuid> otherMembers, QByteArray masterKey, QObject* parent)
    : QObject(parent),
      m_id(std::move(id)),
      m_otherMembers(std::move(otherMembers)),
      m_masterKey(std::move(masterKey)),
      m_masterKeyRequestStarted(false),
      m_masterKeyRequestSent(false),
      m_messageSender(new MessageSender(m_id))
{
    initialize();
}

void Chat::initialize()
{
    m_messageSender->setChatMasterKey(m_masterKey);
    m_messageSender->moveToThread(&m_senderThread);

    connect(&m_senderThread, &QThread::finished, m_messageSender, &QObject::deleteLater);
    connect(this, &Chat::messageSubmitted, m_messageSender, &MessageSender::processMessage);
    connect(m_messageSender, &MessageSender::messageSent, this, &Chat::onMessageSent);

    connect(&RequestManager::instance(), &RequestManager::chatMessageReceived, this, &Chat::onNewMessage);
    connect(&RequestManager::instance(), &RequestManager::userSessionReceived, this, [this](const shared::SessionInfo& sessionInfo) {
        if (!m_pendingMasterKeyUserIds.contains(sessionInfo.userId()))
        {
            if (!m_pendingMasterKeyRequestUserIds.contains(sessionInfo.userId()))
                return;

            m_pendingMasterKeyRequestUserIds.remove(sessionInfo.userId());
            if (!sessionInfo.sessionId().isNull())
            {
                m_masterKeyRequestSent = true;
                RequestManager::instance().requestChatMasterKey(sessionInfo.sessionId(), m_id);
                setKeyStatusText("Waiting for chat key from online members...");
            }
            else if (m_pendingMasterKeyRequestUserIds.isEmpty() && !m_masterKeyRequestSent && !hasMasterKey())
            {
                setKeyStatusText("Chat key unavailable. Need a chat member online.");
            }

            return;
        }

        m_pendingMasterKeyUserIds.remove(sessionInfo.userId());
        if (!sessionInfo.sessionId().isNull())
            RequestManager::instance().sendChatMasterKey(sessionInfo.sessionId(), m_id, m_masterKey);
    });
    connect(&RequestManager::instance(), &RequestManager::chatKeyRequested, this, [this](const QUuid& chatId, const QUuid& requesterSessionId) {
        if (chatId != m_id || requesterSessionId.isNull() || !hasMasterKey())
            return;

        const QUuid requesterUserId = SessionResolver::instance().sessionUserId(requesterSessionId);
        if (!m_otherMembers.contains(requesterUserId))
        {
            qWarning() << "Rejected chat key request from non-member session" << requesterSessionId << "for chat" << m_id;
            return;
        }

        RequestManager::instance().sendChatMasterKey(requesterSessionId, m_id, m_masterKey);
    });

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

    if (!hasMasterKey())
    {
        m_pendingOutgoingMessages.append(message);
        return;
    }

    emit messageSubmitted(message);
}

QString Chat::label() const
{
    QString label;

    for (const QUuid& userId : m_otherMembers)
    {
        QString displayName = "Unknown";

        if (!userId.isNull())
        {
            const auto userInfo = UserResolver::instance().resolveUser(userId);
            displayName = userInfo.has_value() ? userInfo->displayName() : "Loading...";
        }

        if (!label.isEmpty())
            label.append(", ");

        label.append(displayName);
    }

    return label;
}

void Chat::setOtherMembers(QSet<QUuid> otherMembers)
{
    if (m_otherMembers == otherMembers)
        return;

    m_otherMembers = std::move(otherMembers);

    for (const QUuid& userId : m_otherMembers)
        UserResolver::instance().resolveUser(userId);

    emit labelChanged();
}

void Chat::setMasterKey(QByteArray masterKey)
{
    if (masterKey.isEmpty() || m_masterKey == masterKey)
        return;

    m_masterKey = std::move(masterKey);
    m_pendingMasterKeyRequestUserIds.clear();
    m_masterKeyRequestSent = false;
    setKeyStatusText("");
    QMetaObject::invokeMethod(
        m_messageSender,
        [sender = m_messageSender, masterKey = m_masterKey] {
            sender->setChatMasterKey(masterKey);
        },
        Qt::QueuedConnection
    );
    flushPendingOutgoingMessages();
    flushPendingIncomingMessages();
}

void Chat::distributeMasterKey()
{
    if (!hasMasterKey())
        return;

    for (const QUuid& userId : m_otherMembers)
        sendMasterKeyToUser(userId);
}

void Chat::sendMasterKeyToUser(const QUuid& userId)
{
    if (userId.isNull() || !hasMasterKey())
        return;

    const QUuid sessionId = SessionResolver::instance().userSessionId(userId);
    if (!sessionId.isNull())
    {
        RequestManager::instance().sendChatMasterKey(sessionId, m_id, m_masterKey);
        return;
    }

    m_pendingMasterKeyUserIds.insert(userId);
    RequestManager::instance().getUserSession(userId);
}

void Chat::requestMasterKeyFromMembers()
{
    if (hasMasterKey() || m_masterKeyRequestStarted)
        return;

    m_masterKeyRequestStarted = true;
    m_masterKeyRequestSent = false;
    m_pendingMasterKeyRequestUserIds.clear();

    if (m_otherMembers.isEmpty())
    {
        setKeyStatusText("Chat key unavailable. Need a chat member online.");
        return;
    }

    setKeyStatusText("Looking for an online chat member to get the key...");
    for (const QUuid& userId : m_otherMembers)
        requestMasterKeyFromUser(userId);
}

void Chat::requestMasterKeyFromUser(const QUuid& userId)
{
    if (userId.isNull() || hasMasterKey())
        return;

    const QUuid sessionId = SessionResolver::instance().userSessionId(userId);
    if (!sessionId.isNull())
    {
        m_masterKeyRequestSent = true;
        RequestManager::instance().requestChatMasterKey(sessionId, m_id);
        setKeyStatusText("Waiting for chat key from online members...");
        return;
    }

    m_pendingMasterKeyRequestUserIds.insert(userId);
    RequestManager::instance().getUserSession(userId);
}

void Chat::onNewMessage(const shared::Message& messagePacket)
{
    if (messagePacket.targetChatId() != m_id) return;
    if (messagePacket.type() != shared::MessageType::TEXT) return;

    if (!hasMasterKey())
    {
        m_pendingIncomingMessages.append(messagePacket);
        requestMasterKeyFromMembers();
        return;
    }

    handleMessage(messagePacket);
}

void Chat::handleMessage(const shared::Message& messagePacket)
{
    const QUuid& senderUserId = messagePacket.senderUserId();
    qInfo() << "Incoming text message from" << senderUserId;

    QString content = messagePacket.content(m_masterKey);
    auto* chatMessage = new ChatMessage(false, std::move(content), senderUserId, this);

    addChatMessage(chatMessage);

    onMessageReceived(chatMessage->id());
}

void Chat::flushPendingOutgoingMessages()
{
    if (!hasMasterKey())
        return;

    const QList<ChatMessage*> messages = std::move(m_pendingOutgoingMessages);
    m_pendingOutgoingMessages.clear();

    for (ChatMessage* message : messages)
    {
        if (message != nullptr)
            emit messageSubmitted(message);
    }
}

void Chat::flushPendingIncomingMessages()
{
    if (!hasMasterKey())
        return;

    const QList<shared::Message> messages = std::move(m_pendingIncomingMessages);
    m_pendingIncomingMessages.clear();

    for (const shared::Message& message : messages)
        handleMessage(message);
}

void Chat::setKeyStatusText(QString keyStatusText)
{
    if (m_keyStatusText == keyStatusText)
        return;

    m_keyStatusText = std::move(keyStatusText);
    emit keyStatusTextChanged();
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
