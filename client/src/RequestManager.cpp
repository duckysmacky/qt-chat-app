#include "RequestManager.h"

#include <QDebug>
#include <QUuid>

#include <utility>

#include "AccountManager.h"
#include "Client.h"
#include "KeyStore.h"
#include "PacketFactory.h"
#include "SessionResolver.h"
#include "crypto.h"
#include "dto/AuthInfo.h"
#include "dto/SessionInfo.h"
#include "util.h"

RequestManager& RequestManager::instance()
{
    static RequestManager instance;
    return instance;
}

RequestManager::RequestManager(QObject* parent)
    : QObject(parent)
{}

void RequestManager::processBytes(const QByteArray& bytes)
{
    m_incomingBuffer.append(bytes);
    const QList<shared::Packet> packets = shared::util::parseStream(m_incomingBuffer);
    for (const auto& packet : packets)
        processPacket(packet);
}

void RequestManager::processPacket(const shared::Packet& packet)
{
    switch (packet.type())
    {
    case shared::PacketType::INVALID:
        emit invalidPacketReceived(packet);
        break;

    case shared::PacketType::CONNECT:
        SessionResolver::instance().setServerSessionId(packet.sender());
        requestKeyExchange(packet.sender());
        break;

    case shared::PacketType::KEY_EXCHANGE:
        handleKeyExchange(packet);
        break;

    case shared::PacketType::CHAT_MESSAGE:
        {
            if (const auto payload = decryptPayload(packet))
            {
                const auto message = shared::Message::deserialize(payload.value());
                emit chatMessageReceived(message);
            }
        }
        break;

    case shared::PacketType::OPERATION_RESULT:
        {
            const auto payload = decryptPayload(packet);
            if (!payload.has_value()) break;

            const shared::OperationResult result = shared::OperationResult::deserialize(payload.value());
            const bool success = result.type() == shared::OperationResultType::SUCCESS;

            if (success)
                qInfo() << "Server result:" << result.text();
            else
                qWarning() << "Server result:" << result.text();

            emit operationResultReceived(result);
        }
        break;

    case shared::PacketType::USER_PROFILE_DATA:
        {
            const auto payload = decryptPayload(packet);
            if (!payload.has_value()) {
                qWarning() << "User profile data payload is missing";
                break;
            }

            const auto profile = shared::ProfileInfo::deserialize(payload.value());
            if (!profile.has_value()) {
                qWarning() << "Invalid user profile data payload";
                break;
            }

            emit currentUserProfileReceived(profile.value());
        }
        break;

    case shared::PacketType::PUBLIC_USER_INFO_DATA:
        {
            const auto payload = decryptPayload(packet);
            if (!payload.has_value()) {
                qWarning() << "Public user info data payload is missing";
                break;
            }

            const auto userInfo = shared::PublicUserInfo::deserialize(payload.value());
            if (!userInfo.has_value()) {
                qWarning() << "Invalid public user info data payload";
                break;
            }

            emit publicUserInfoReceived(userInfo.value());
        }
        break;

    case shared::PacketType::CHAT_LIST_DATA:
        {
            const auto payload = decryptPayload(packet);
            if (!payload.has_value()) {
                qWarning() << "Chat list data payload is missing";
                break;
            }

            const auto chats = shared::ChatsInfo::deserialize(payload.value());
            if (!chats.has_value()) {
                qWarning() << "Invalid chat list data payload";
                break;
            }

            emit chatListReceived(chats.value());
        }
        break;

    case shared::PacketType::CHAT_INFO_DATA:
        {
            const auto payload = decryptPayload(packet);
            if (!payload.has_value()) {
                qWarning() << "Chat info data payload is missing";
                break;
            }

            const auto chat = shared::ChatInfo::deserialize(payload.value());
            if (!chat.has_value()) {
                qWarning() << "Invalid chat info data payload";
                break;
            }

            emit chatInfoReceived(chat.value());
        }
        break;

    case shared::PacketType::USER_SESSION_DATA:
        {
            const auto payload = decryptPayload(packet);
            if (!payload.has_value()) {
                qWarning() << "User session data payload is missing";
                break;
            }

            const auto sessionInfo = shared::SessionInfo::deserialize(payload.value());
            if (!sessionInfo.has_value()) {
                qWarning() << "Invalid user session data payload";
                break;
            }

            SessionResolver::instance().setUserSessionId(sessionInfo->userId(), sessionInfo->sessionId());
            emit userSessionReceived(sessionInfo.value());
        }
        break;

    default:
        qWarning() << "Unknown or unsupported packet received";
        emit unsupportedPacketReceived(packet);
        break;
    }
}

void RequestManager::connectClient() const
{
    const Client& client = Client::instance();
    sendPlainPacket(shared::PacketFactory::connectPacket(client.sessionId(), QUuid()));
}

void RequestManager::sendServerCommand() const
{
    const Client& client = Client::instance();
    sendPlainPacket(shared::Packet(shared::PacketType::SERVER_COMMAND, client.sessionId(), SessionResolver::instance().serverSessionId()));
}

void RequestManager::sendServerCommand(QByteArray data) const
{
    const Client& client = Client::instance();
    const QUuid serverSessionId = SessionResolver::instance().serverSessionId();
    sendEncryptedPacket(shared::Packet(shared::PacketType::SERVER_COMMAND, client.sessionId(), serverSessionId), std::move(data));
}

void RequestManager::sendChatMessage(shared::Message message) const
{
    const Client& client = Client::instance();
    const QUuid serverSessionId = SessionResolver::instance().serverSessionId();
    sendEncryptedPacket(shared::Packet(shared::PacketType::CHAT_MESSAGE, client.sessionId(), serverSessionId), message.serialize());
}

void RequestManager::sendTextChatMessage(const QUuid& targetChatId, QString content) const
{
    QUuid senderUserId;
    if (const auto& userId = AccountManager::instance().userId(); userId.has_value())
        senderUserId = userId.value();

    const QUuid serverSessionId = SessionResolver::instance().serverSessionId();
    const auto serverKey = shared::KeyStore::instance().peerPublicKey(serverSessionId);
    if (!serverKey.has_value()) {
        requestKeyExchange(serverSessionId);
        return;
    }

    shared::Message message(senderUserId, targetChatId, shared::MessageType::TEXT);
    message.setContent(content, serverKey.value());
    sendChatMessage(std::move(message));
}

void RequestManager::sendMediaChatMessage(const QUuid& targetChatId, QString content) const
{
    QUuid senderUserId;
    if (const auto& userId = AccountManager::instance().userId(); userId.has_value())
        senderUserId = userId.value();

    const QUuid serverSessionId = SessionResolver::instance().serverSessionId();
    const auto serverKey = shared::KeyStore::instance().peerPublicKey(serverSessionId);
    if (!serverKey.has_value()) {
        requestKeyExchange(serverSessionId);
        return;
    }

    shared::Message message(senderUserId, targetChatId, shared::MessageType::MEDIA);
    message.setContent(content, serverKey.value());
    sendChatMessage(std::move(message));
}

void RequestManager::loginUser(QString login, QString passwordHash) const
{
    const Client& client = Client::instance();
    const QUuid serverSessionId = SessionResolver::instance().serverSessionId();
    const shared::LoginInfo info(std::move(login), std::move(passwordHash));
    sendEncryptedPacket(shared::Packet(shared::PacketType::LOGIN_USER, client.sessionId(), serverSessionId), info.serialize());
}

void RequestManager::registerUser(QString username, QString displayName, QString email, QString passwordHash) const
{
    const Client& client = Client::instance();
    const QUuid serverSessionId = SessionResolver::instance().serverSessionId();
    const shared::RegisterInfo info(
        std::move(username),
        std::move(displayName),
        std::move(email),
        std::move(passwordHash)
    );
    sendEncryptedPacket(shared::Packet(shared::PacketType::REGISTER_USER, client.sessionId(), serverSessionId), info.serialize());
}

void RequestManager::logoutCurrentUser() const
{
    const Client& client = Client::instance();
    sendPlainPacket(shared::Packet(shared::PacketType::LOGOUT_USER, client.sessionId(), SessionResolver::instance().serverSessionId()));
}

void RequestManager::getCurrentUserProfile() const
{
    const Client& client = Client::instance();
    sendPlainPacket(shared::PacketFactory::getUserProfilePacket(client.sessionId(), SessionResolver::instance().serverSessionId()));
}

void RequestManager::updateCurrentUserProfile(shared::ProfileUpdateInfo info) const
{
    const Client& client = Client::instance();
    const QUuid serverSessionId = SessionResolver::instance().serverSessionId();
    sendEncryptedPacket(shared::Packet(shared::PacketType::UPDATE_USER_PROFILE, client.sessionId(), serverSessionId), info.serialize());
}

void RequestManager::getUserInfo(shared::UserInfoRequest request) const
{
    const Client& client = Client::instance();
    const QUuid serverSessionId = SessionResolver::instance().serverSessionId();
    sendEncryptedPacket(shared::Packet(shared::PacketType::GET_USER_INFO, client.sessionId(), serverSessionId), request.serialize());
}

void RequestManager::getUserInfo(const QUuid& userId) const
{
    getUserInfo(shared::UserInfoRequest(userId));
}

void RequestManager::getUserInfo(QString username) const
{
    username = username.trimmed();
    if (username.startsWith('@'))
        username.remove(0, 1);

    getUserInfo(shared::UserInfoRequest(std::move(username)));
}

void RequestManager::getPublicUserInfo(const QUuid& userId) const
{
    getUserInfo(userId);
}

void RequestManager::getUserSession(const QUuid& userId) const
{
    const Client& client = Client::instance();
    const QUuid serverSessionId = SessionResolver::instance().serverSessionId();
    sendEncryptedPacket(shared::Packet(shared::PacketType::GET_USER_SESSION, client.sessionId(), serverSessionId), userId.toRfc4122());
}

void RequestManager::getCurrentUserChats() const
{
    const Client& client = Client::instance();
    sendPlainPacket(shared::PacketFactory::getChatsPacket(client.sessionId(), SessionResolver::instance().serverSessionId()));
}

void RequestManager::searchChats(QString query) const
{
    const Client& client = Client::instance();
    const QUuid serverSessionId = SessionResolver::instance().serverSessionId();
    sendEncryptedPacket(shared::Packet(shared::PacketType::SEARCH_CHATS, client.sessionId(), serverSessionId), query.toUtf8());
}

void RequestManager::createChat(shared::ChatCreateInfo info) const
{
    const Client& client = Client::instance();
    const QUuid serverSessionId = SessionResolver::instance().serverSessionId();
    sendEncryptedPacket(shared::Packet(shared::PacketType::CREATE_CHAT, client.sessionId(), serverSessionId), info.serialize());
}

void RequestManager::createChat(QList<QUuid> memberIds) const
{
    createChat(shared::ChatCreateInfo(std::move(memberIds)));
}

void RequestManager::sendPlainPacket(shared::Packet packet) const
{
    Client::instance().sendBytes(shared::util::encapsulate(packet));
}

void RequestManager::sendEncryptedPacket(shared::Packet packet, QByteArray payload) const
{
    const QUuid receiverSessionId = packet.receiver();
    if (receiverSessionId.isNull())
    {
        qWarning() << "Cannot encrypt packet without receiver session id";
        return;
    }

    const auto encryptionKey = shared::KeyStore::instance().peerPublicKey(receiverSessionId);
    if (encryptionKey.has_value())
    {
        packet.setPayload(payload, encryptionKey.value());
        sendPlainPacket(std::move(packet));
    }
    else
    {
        m_packetQueue.append(QueuedPacket{std::move(packet), std::move(payload)});
        requestKeyExchange(receiverSessionId);
    }
}

void RequestManager::requestKeyExchange(const QUuid& receiverSessionId) const
{
    if (receiverSessionId.isNull())
    {
        qWarning() << "Cannot request key exchange without receiver session id";
        return;
    }

    if (m_requestedKeyExchanges.contains(receiverSessionId))
        return;

    const auto derivedKeyPair = shared::crypto::deriveKeyPair(receiverSessionId);
    const auto packet = shared::PacketFactory::keyExchangePacket(
        Client::instance().sessionId(),
        receiverSessionId,
        shared::KeyStore::instance().publicKey(),
        derivedKeyPair.first
    );

    m_requestedKeyExchanges.insert(receiverSessionId);
    sendPlainPacket(packet);
}

void RequestManager::handleKeyExchange(const shared::Packet& packet)
{
    const bool hadPeerKey = shared::KeyStore::instance().hasPeerPublicKey(packet.sender());
    const bool requestedPeerKey = m_requestedKeyExchanges.contains(packet.sender());
    const auto payload = decryptPayload(packet);
    if (!payload.has_value() || payload->isEmpty())
    {
        qWarning() << "Invalid key exchange payload";
        return;
    }

    shared::KeyStore::instance().setPeerPublicKey(packet.sender(), payload.value());
    m_requestedKeyExchanges.remove(packet.sender());

    if (!hadPeerKey && !requestedPeerKey)
        requestKeyExchange(packet.sender());

    flushQueuedPackets(packet.sender());
}

void RequestManager::flushQueuedPackets(const QUuid& receiverSessionId) const
{
    const auto encryptionKey = shared::KeyStore::instance().peerPublicKey(receiverSessionId);
    if (!encryptionKey.has_value())
        return;

    for (auto it = m_packetQueue.begin(); it != m_packetQueue.end();)
    {
        if (it->packet.receiver() != receiverSessionId)
        {
            ++it;
            continue;
        }

        shared::Packet packet = std::move(it->packet);
        packet.setPayload(it->payload, encryptionKey.value());
        sendPlainPacket(std::move(packet));
        it = m_packetQueue.erase(it);
    }
}

std::optional<QByteArray> RequestManager::decryptPayload(const shared::Packet& packet) const
{
    if (!packet.hasPayload())
        return std::nullopt;

    const QByteArray decryptionKey = packet.type() == shared::PacketType::KEY_EXCHANGE
        ? shared::crypto::deriveKeyPair(packet.receiver()).second
        : shared::KeyStore::instance().privateKey();

    return packet.payload(decryptionKey);
}