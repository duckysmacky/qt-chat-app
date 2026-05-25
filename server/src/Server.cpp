#include "Server.h"

#include <QByteArray>
#include <QDebug>
#include <QSet>
#include <qlogging.h>

#include "dto/AuthInfo.h"
#include "Message.h"
#include "Database.h"
#include "PacketFactory.h"
#include "dto/ProfileInfo.h"
#include "dto/ProfileUpdateInfo.h"
#include "dto/PublicUserInfo.h"
#include "dto/UserInfoRequest.h"
#include "dto/SessionInfo.h"
#include "dto/ChatInfo.h"
#include "dto/ChatsInfo.h"
#include "dto/CreateChatInfo.h"
#include "model/ChatMember.h"
#include "model/Chat.h"

#include "KeyStore.h"
#include "crypto.h"
#include "util.h"

namespace {

shared::ChatInfo makeChatInfo(const Database& db, const model::Chat& chat)
{
    QUuid id = chat.id();
    QString type = model::chatTypeToString(chat.type());
    QUuid createdBy = chat.createdBy();
    QDateTime createdAt = chat.createdAt();
    QList<QUuid> memberIds = db.getUserIdsByChatId(id);

    return shared::ChatInfo(
        std::move(id),
        std::move(type),
        std::move(createdBy),
        std::move(createdAt),
        std::move(memberIds)
    );
}

shared::ChatsInfo makeChatsInfo(const Database& db, const QList<model::Chat>& chats)
{
    QList<shared::ChatInfo> chatInfos;
    chatInfos.reserve(chats.size());

    for (const auto& chat : chats)
        chatInfos.append(makeChatInfo(db, chat));

    return shared::ChatsInfo(std::move(chatInfos));
}

std::optional<QByteArray> decryptPacketPayload(const shared::Packet& packet)
{
    if (!packet.hasPayload()) return std::nullopt;

    const QByteArray decryptionKey = packet.type() == shared::PacketType::PUBLIC_KEY_EXCHANGE
        ? shared::crypto::deriveKeyPair(packet.receiver()).second
        : shared::KeyStore::instance().privateKey();

    return packet.payload(decryptionKey);
}

std::optional<QByteArray> peerEncryptionKey(const QUuid& sessionId)
{
    return shared::KeyStore::instance().peerPublicKey(sessionId);
}

}

/**
 * @brief Returns the singleton Server instance
 */
Server& Server::instance()
{
	static Server instance;
	return instance;
}

Server::Server(QObject* parent)
	: QObject(parent),
      m_uuid(QUuid::createUuid()),
	  m_server(new QTcpServer(this)),
	  m_isRunning(false)
{
	connect(m_server, &QTcpServer::newConnection, this, &Server::onNewConnection);
}

Server::~Server()
{
	stop();
}

bool Server::start(const uint16_t port)
{

	if (m_server->listen(QHostAddress::Any, port))
	{
		qInfo() << "Server started and listening on port" << port;
		m_isRunning = true;
		return true;
	}

	qCritical() << "Server not started";
	return false;
}

void Server::stop() const
{
	if (m_isRunning)
		m_server->close();
}

/**
 * @brief Sends a packet to a specific client
 */
void Server::sendPacket(const QUuid& receiverSessionId, const shared::Packet& packet) const
{
    auto it = m_clients.constFind(receiverSessionId);
    if (it == m_clients.constEnd()) {
        qWarning() << "Receiver session not found:" << receiverSessionId.toString();
        return;
    }

    if (!it.value().sendPacket(packet)) {
        qCritical() << "Error writing to" << receiverSessionId.toString();
    }
}

void Server::sendChatListData(const QUuid& receiverSessionId, const shared::ChatsInfo& info) const
{
    const auto encryptionKey = peerEncryptionKey(receiverSessionId);
    if (!encryptionKey.has_value()) return;

    const auto packet = shared::PacketFactory::chatListDataPacket(m_uuid, receiverSessionId, info, encryptionKey.value());
    sendPacket(receiverSessionId, packet);
}

void Server::sendChatInfoData(const QUuid& receiverSessionId, const shared::ChatInfo& info) const
{
    const auto encryptionKey = peerEncryptionKey(receiverSessionId);
    if (!encryptionKey.has_value()) return;

    const auto packet = shared::PacketFactory::chatInfoDataPacket(m_uuid, receiverSessionId, info, encryptionKey.value());
    sendPacket(receiverSessionId, packet);
}

void Server::sendUpdatedChatLists(const QSet<QUuid>& memberUserIds) const
{
    const Database& db = Database::instance();

    for (const auto& connection : m_clients)
    {
        if (!connection.isAuthorized() || !connection.userId().has_value())
            continue;

        const QUuid userId = connection.userId().value();
        if (!memberUserIds.contains(userId))
            continue;

        sendChatListData(connection.sessionId(), makeChatsInfo(db, db.getChatsByUserId(userId)));
    }
}

void Server::sendError(const QUuid& receiverSessionId, QString message) const
{
    const auto encryptionKey = peerEncryptionKey(receiverSessionId);
    if (!encryptionKey.has_value()) return;

	auto packet = shared::PacketFactory::operationErrorPacket(m_uuid, receiverSessionId, std::move(message), encryptionKey.value());
	sendPacket(receiverSessionId, packet);
}

void Server::sendSuccess(const QUuid& receiverSessionId, QString message) const
{
    const auto encryptionKey = peerEncryptionKey(receiverSessionId);
    if (!encryptionKey.has_value()) return;

	auto packet = shared::PacketFactory::operationSuccessPacket(m_uuid, receiverSessionId, std::move(message), encryptionKey.value());
	sendPacket(receiverSessionId, packet);
}

void Server::sendUserProfileData(const QUuid& receiverSessionId, const shared::ProfileInfo& info) const
{
    const auto encryptionKey = peerEncryptionKey(receiverSessionId);
    if (!encryptionKey.has_value()) return;

    const auto packet = shared::PacketFactory::userProfileDataPacket(m_uuid, receiverSessionId, info, encryptionKey.value());
    sendPacket(receiverSessionId, packet);
}

void Server::sendPublicUserInfoData(const QUuid& receiverSessionId, const shared::PublicUserInfo& info) const
{
    const auto encryptionKey = peerEncryptionKey(receiverSessionId);
    if (!encryptionKey.has_value()) return;

    const auto packet = shared::PacketFactory::publicUserInfoDataPacket(m_uuid, receiverSessionId, info, encryptionKey.value());
    sendPacket(receiverSessionId, packet);
}

void Server::sendUserSessionData(const QUuid& receiverSessionId, const shared::SessionInfo& info) const
{
    const auto encryptionKey = peerEncryptionKey(receiverSessionId);
    if (!encryptionKey.has_value()) return;

    const auto packet = shared::PacketFactory::userSessionDataPacket(m_uuid, receiverSessionId, info, encryptionKey.value());
    sendPacket(receiverSessionId, packet);
}

/**
 * Handles new incoming TCP connections; returns None if socket is not defined
 */
void Server::onNewConnection()
{
	QTcpSocket* socket = m_server->nextPendingConnection();
	if (!socket) return;

    m_socketBuffers.insert(socket, {});
	connect(socket, &QTcpSocket::readyRead, this, &Server::onServerRead);
	connect(socket, &QTcpSocket::disconnected, this, &Server::onClientDisconnected);
}

void Server::onServerRead()
{
	auto* socket = qobject_cast<QTcpSocket*>(sender());
	if (!socket) return;

    while (socket->bytesAvailable() > 0)
    {
        const QByteArray bytes = socket->readAll();
        QByteArray& buffer = m_socketBuffers[socket];
        buffer.append(bytes);
        const QList<shared::Packet> packets = shared::util::parseStream(buffer);

        for (const auto& packet : packets)
        {
            if (packet.receiver() != m_uuid)
            {
                if (packet.type() == shared::PacketType::CONNECT)
                    handleConnect(socket, packet);
                else
                    sendPacket(packet.receiver(), packet);

                continue;
            }

            switch (packet.type())
            {
            case shared::PacketType::REGISTER_USER:
                handleRegisterUser(socket, packet);
                break;

            case shared::PacketType::LOGIN_USER:
                handleLoginUser(socket, packet);
                break;

            case shared::PacketType::LOGOUT_USER:
                handleLogoutUser(socket, packet);
                break;

            case shared::PacketType::GET_USER_PROFILE:
                handleGetUserProfile(socket, packet);
                break;

            case shared::PacketType::UPDATE_USER_PROFILE:
                handleUpdateUserProfile(socket, packet);
                break;

            case shared::PacketType::GET_USER_INFO:
                handleGetUserInfo(socket, packet);
                break;

            case shared::PacketType::GET_USER_SESSION:
                handleGetUserSession(socket, packet);
                break;

            case shared::PacketType::GET_CHATS:
                handleGetChats(socket, packet);
                break;

            case shared::PacketType::SEARCH_CHATS:
                handleSearchChats(socket, packet);
                break;

            case shared::PacketType::CREATE_CHAT:
                handleCreateChat(socket, packet);
                break;

            case shared::PacketType::PUBLIC_KEY_EXCHANGE:
                handleKeyExchange(socket, packet);
                break;

            default:
                handleAuthorizedPacket(packet);
                break;
            }
		}
    }
}

void Server::handleGetChats(const QTcpSocket* socket, const shared::Packet& packet)
{
    if (!socket) return;
    if (packet.type() != shared::PacketType::GET_CHATS) return;

    const auto connectionOpt = findConnection(packet.sender());
    if (!connectionOpt.has_value()) {
        qWarning() << "Client" << packet.sender() << "not yet connected";
        return;
    }

    const ClientConnection& connection = connectionOpt->get();

    if (!connection.matchesSocket(socket) || !connection.isAuthorized() || !connection.userId().has_value()) {
        sendError(connection.sessionId(), "Not authorized");
        return;
    }

    const Database& db = Database::instance();
    const QList<model::Chat> chats = db.getChatsByUserId(connection.userId().value());

    sendChatListData(connection.sessionId(), makeChatsInfo(db, chats));
}


/**
 * @brief Handles client disconnection
 */
void Server::onClientDisconnected()
{
    auto* clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket)
        return;

    m_socketBuffers.remove(clientSocket);

    for (auto it = m_clients.begin(); it != m_clients.end(); ++it)
    {
        ClientConnection& connection = it.value();

        if (!connection.matchesSocket(clientSocket))
            continue;

        const QUuid disconnectedSessionId = it.key();

        if (connection.isAuthorized())
        {
            const QString userIdText = connection.userId().has_value()
            ? connection.userId().value().toString()
            : QString("<unknown>");

            qInfo() << "Logging out authorized client before disconnect:"
                    << "session =" << disconnectedSessionId.toString()
                    << "user =" << userIdText;

            connection.logout();
        }

        qInfo() << "Client session" << disconnectedSessionId.toString() << "disconnected";
        shared::KeyStore::instance().removePeerPublicKey(disconnectedSessionId);
        m_clients.erase(it);
        break;
    }

    clientSocket->close();
    clientSocket->deleteLater();
}

std::optional<std::reference_wrapper<ClientConnection>> Server::findConnection(const QUuid& sessionId)
{
    const auto it = m_clients.find(sessionId);

    if (it == m_clients.end())
        return std::nullopt;

    return it.value();
}

std::optional<std::reference_wrapper<const ClientConnection>> Server::findConnection(const QUuid& sessionId) const
{
    const auto it = m_clients.find(sessionId);

    if (it == m_clients.end())
        return std::nullopt;

    return it.value();
}

std::optional<std::reference_wrapper<ClientConnection>> Server::findConnection(const QTcpSocket* clientSocket)
{
    for (auto it = m_clients.begin(); it != m_clients.end(); ++it)
    {
        if (it.value().matchesSocket(clientSocket))
            return it.value();
    }

    return std::nullopt;
}

std::optional<std::reference_wrapper<const ClientConnection>> Server::findConnection(const QTcpSocket* clientSocket) const
{
    for (auto it = m_clients.begin(); it != m_clients.end(); ++it)
    {
        if (it.value().matchesSocket(clientSocket))
            return it.value();
    }

    return std::nullopt;
}

void Server::handleConnect(QTcpSocket* socket, const shared::Packet& packet)
{
    if (!socket) return;
    if (packet.type() != shared::PacketType::CONNECT) return;

    const QUuid sessionId = packet.sender();

    if (m_clients.contains(sessionId))
    {
        qWarning() << "Client" << sessionId << "already connected";
        return;
    }

    m_clients.insert(sessionId, ClientConnection(sessionId, socket));

    sendPacket(sessionId, shared::PacketFactory::connectPacket(m_uuid, sessionId));
    qInfo() << "Connected a new client" << sessionId;
}

void Server::handleRegisterUser(const QTcpSocket* socket, const shared::Packet& packet)
{
    if (!socket) return;
    if (packet.type() != shared::PacketType::REGISTER_USER) return;

    const auto connectionOpt = findConnection(packet.sender());

    if (!connectionOpt.has_value())
    {
        qWarning() << "Client" << packet.sender() << "not yet connected, cannot register unconnected client";
        return;
    }

    const ClientConnection& connection = connectionOpt->get();

    if (!connection.matchesSocket(socket) || !packet.hasPayload()) return;

    const auto payload = decryptPacketPayload(packet);
    if (!payload.has_value()) {
        sendError(connection.sessionId(), "Unable to decrypt register payload");
        return;
    }

    const auto registerInfo = shared::RegisterInfo::deserialize(payload.value());

    if (!registerInfo.has_value())
    {
        qWarning() << "Client" << packet.sender() << "sent invalid register payload";
        sendError(connection.sessionId(), "Invalid register payload");
        return;
    }

    Database& db = Database::instance();
    const QString username = registerInfo->username().trimmed();
    const QString displayName = registerInfo->displayName().trimmed();
    const QString email = registerInfo->email().trimmed();

    if (!shared::util::isValidUsername(username)) {
        sendError(connection.sessionId(), "Username must be 2-20 characters and contain only lowercase latin letters, numbers, and underscores");
        return;
    }

    if (displayName.isEmpty()) {
        sendError(connection.sessionId(), "Display name must not be empty");
        return;
    }

    if (db.getUserByUsername(username).has_value())
    {
        qWarning() << "Username" << username << "already exists";
        sendError(connection.sessionId(), "Username already exists");
        return;
    }

    const model::User user(
        username,
        displayName,
        registerInfo->passwordHash(),
        email
    );

    if (!db.createUser(user))
    {
        qCritical() << "Registration of client" << packet.sender() << "failed";
        sendError(connection.sessionId(), "Registration failed");
        return;
    }

    sendSuccess(connection.sessionId(), "Registration successful");
    qInfo() << "Successfully registered client" << packet.sender() << "as" << username;
}

void Server::handleLoginUser(const QTcpSocket* socket, const shared::Packet& packet)
{
    if (!socket) return;
    if (packet.type() != shared::PacketType::LOGIN_USER) return;

    const auto connectionOpt = findConnection(packet.sender());

    if (!connectionOpt.has_value())
    {
        qWarning() << "Client" << packet.sender() << "not yet connected, cannot log in unconnected client";
        return;
    }

    ClientConnection& connection = connectionOpt->get();

    if (!connection.matchesSocket(socket) || !packet.hasPayload()) return;

    const auto payload = decryptPacketPayload(packet);
    if (!payload.has_value()) {
        sendError(connection.sessionId(), "Unable to decrypt login payload");
        return;
    }

    const auto loginInfo = shared::LoginInfo::deserialize(payload.value());

    if (!loginInfo.has_value())
    {
        qWarning() << "Client" << packet.sender() << "sent invalid login payload";
        sendError(connection.sessionId(), "Invalid login payload");
        return;
    }

    const Database& db = Database::instance();
    const auto user = db.authenticateUser(loginInfo.value());

    if (!user.has_value())
    {
        sendError(connection.sessionId(), "Invalid login or password");
        return;
    }

    for (const ClientConnection& existingConnection : m_clients)
    {
        if (!existingConnection.isAuthorized() || !existingConnection.userId().has_value())
            continue;

        if (existingConnection.sessionId() == connection.sessionId())
            continue;

        if (existingConnection.userId().value() != user->id())
            continue;

        sendError(connection.sessionId(), "User is already logged in from another device");
        qWarning() << "Rejected duplicate login for user" << user->id().toString()
                   << "from session" << connection.sessionId().toString();
        return;
    }

    connection.authorize(user->id());

    sendSuccess(connection.sessionId(), "Login successful");
    qInfo() << "Successfully logged in client" << packet.sender();
}

void Server::handleAuthorizedPacket(const shared::Packet& packet) const
{
    const QUuid& sessionId = packet.sender();

    if (sessionId.isNull())
    {
        qWarning() << "Ignoring packet with invalid UUID";
        return;
    }

    auto connectionOpt = findConnection(sessionId);
    if (!connectionOpt.has_value())
    {
        qWarning() << "Ignoring packet from unconnected client";
        return;
    }

    const ClientConnection& connection = connectionOpt.value().get();

    if (!connection.isAuthorized())
    {
        qWarning() << "Declining a packet from unauthorized client";
        sendError(sessionId, "Not authorized");
        return;
    }

    switch (packet.type())
    {
    case shared::PacketType::CHAT_MESSAGE:
        handleChatMessage(connection, packet);
        break;

    case shared::PacketType::SERVER_COMMAND:
        qInfo() << "Command from" << sessionId.toString();
        break;

    default:
        qInfo() << "Unknown or unsupported message from" << sessionId.toString();
        break;
    }
}

void Server::handleLogoutUser(const QTcpSocket* socket, const shared::Packet& packet)
{
    if (!socket) return;
    if (packet.type() != shared::PacketType::LOGOUT_USER) return;

    const auto connectionOpt = findConnection(packet.sender());

    if (!connectionOpt.has_value())
    {
        qWarning() << "Client" << packet.sender() << "not yet connected";
        return;
    }

    ClientConnection& connection = connectionOpt->get();

    if (!connection.matchesSocket(socket))
        return;

    if (!connection.isAuthorized())
    {
        qWarning() << "Client" << packet.sender() << "is not authorized";
        sendError(connection.sessionId(), "Not authorized");
        return;
    }

    connection.logout();

    sendSuccess(connection.sessionId(), "Logout successful");
    qInfo() << "Successfully logged out client" << packet.sender();
}

void Server::handleSearchChats(const QTcpSocket* socket, const shared::Packet& packet)
{
    if (!socket) return;
    if (packet.type() != shared::PacketType::SEARCH_CHATS) return;

    const auto connectionOpt = findConnection(packet.sender());
    if (!connectionOpt.has_value()) {
        qWarning() << "Client" << packet.sender() << "not yet connected";
        return;
    }

    const ClientConnection& connection = connectionOpt->get();

    if (!connection.matchesSocket(socket) || !connection.isAuthorized()) {
        sendError(connection.sessionId(), "Not authorized");
        return;
    }

    if (!packet.hasPayload()) {
        sendError(connection.sessionId(), "Chat search payload is missing");
        return;
    }

    const auto payload = decryptPacketPayload(packet);
    if (!payload.has_value()) {
        sendError(connection.sessionId(), "Unable to decrypt chat search payload");
        return;
    }

    const QString queryText = QString::fromUtf8(payload.value()).trimmed();

    const Database& db = Database::instance();
    const QList<model::Chat> chats = db.searchChats(queryText);

    sendChatListData(connection.sessionId(), makeChatsInfo(db, chats));
}

void Server::handleCreateChat(const QTcpSocket* socket, const shared::Packet& packet)
{
    if (!socket) return;
    if (packet.type() != shared::PacketType::CREATE_CHAT) return;

    const auto connectionOpt = findConnection(packet.sender());
    if (!connectionOpt.has_value()) {
        qWarning() << "Client" << packet.sender() << "not yet connected";
        return;
    }

    const ClientConnection& connection = connectionOpt->get();

    if (!connection.matchesSocket(socket) || !connection.isAuthorized() || !connection.userId().has_value()) {
        sendError(connection.sessionId(), "Not authorized");
        return;
    }

    if (!packet.hasPayload()) {
        sendError(connection.sessionId(), "Chat create payload is missing");
        return;
    }

    const auto payload = decryptPacketPayload(packet);
    if (!payload.has_value()) {
        sendError(connection.sessionId(), "Unable to decrypt chat create payload");
        return;
    }

    const auto createInfoOpt = shared::ChatCreateInfo::deserialize(payload.value());
    if (!createInfoOpt.has_value()) {
        sendError(connection.sessionId(), "Invalid chat create payload");
        return;
    }

    const QUuid creatorUserId = connection.userId().value();

    Database& db = Database::instance();

    QSet<QUuid> memberIds;
    memberIds.insert(creatorUserId);

    for (const QUuid& requestedMemberId : createInfoOpt->memberIds()) {
        if (requestedMemberId.isNull() || requestedMemberId == creatorUserId)
            continue;

        if (!db.getUserById(requestedMemberId).has_value()) {
            qWarning() << "Skipping invalid chat member user id" << requestedMemberId;
            continue;
        }

        memberIds.insert(requestedMemberId);
    }

    if (memberIds.size() < 2) {
        sendError(connection.sessionId(), "Chat must have at least one valid member");
        return;
    }

    const model::ChatType chatType = memberIds.size() > 2
        ? model::ChatType::Group
        : model::ChatType::Direct;

    model::Chat chat(chatType, creatorUserId);

    if (!db.createChat(chat)) {
        sendError(connection.sessionId(), "Failed to create chat");
        return;
    }

    for (const QUuid& memberId : memberIds) {
        const model::ChatMember membership(chat.id(), memberId);
        if (!db.createChatMember(membership)) {
            db.deleteChat(chat.id());
            sendError(connection.sessionId(), "Failed to add chat member");
            return;
        }
    }

    sendSuccess(connection.sessionId(), "Chat created successfully");
    sendChatInfoData(connection.sessionId(), makeChatInfo(db, chat));
    sendUpdatedChatLists(memberIds);
}


void Server::handleGetUserProfile(const QTcpSocket* socket, const shared::Packet& packet)
{
    if (!socket) return;
    if (packet.type() != shared::PacketType::GET_USER_PROFILE) return;

    const auto connectionOpt = findConnection(packet.sender());
    if (!connectionOpt.has_value()) {
        qWarning() << "Client" << packet.sender() << "not yet connected";
        return;
    }

    const ClientConnection& connection = connectionOpt->get();

    if (!connection.matchesSocket(socket) || !connection.isAuthorized() || !connection.userId().has_value()) {
        sendError(connection.sessionId(), "Not authorized");
        return;
    }

    const Database& db = Database::instance();
    const auto profileInfo = db.getProfileInfoByUserId(connection.userId().value());

    if (!profileInfo.has_value()) {
        sendError(connection.sessionId(), "User not found");
        return;
    }

    sendUserProfileData(connection.sessionId(), profileInfo.value());
}

void Server::handleUpdateUserProfile(const QTcpSocket* socket, const shared::Packet& packet)
{
    if (!socket) return;
    if (packet.type() != shared::PacketType::UPDATE_USER_PROFILE) return;

    const auto connectionOpt = findConnection(packet.sender());
    if (!connectionOpt.has_value()) {
        qWarning() << "Client" << packet.sender() << "not yet connected";
        return;
    }

    const ClientConnection& connection = connectionOpt->get();

    if (!connection.matchesSocket(socket) || !connection.isAuthorized() || !connection.userId().has_value()) {
        sendError(connection.sessionId(), "Not authorized");
        return;
    }

    if (!packet.hasPayload()) {
        sendError(connection.sessionId(), "Profile update payload is missing");
        return;
    }

    const auto payload = decryptPacketPayload(packet);
    if (!payload.has_value()) {
        sendError(connection.sessionId(), "Unable to decrypt profile update payload");
        return;
    }

    const auto updateInfoOpt = shared::ProfileUpdateInfo::deserialize(payload.value());
    if (!updateInfoOpt.has_value()) {
        sendError(connection.sessionId(), "Invalid profile update payload");
        return;
    }

    shared::ProfileUpdateInfo updateInfo = updateInfoOpt.value();

    if (updateInfo.username().has_value()) {
        const QString username = updateInfo.username().value().trimmed();

        if (!shared::util::isValidUsername(username)) {
            sendError(connection.sessionId(), "Username must be 2-20 characters and contain only lowercase latin letters, numbers, and underscores");
            return;
        }

        updateInfo.setUsername(username);
    }

    if (updateInfo.displayName().has_value()) {
        const QString displayName = updateInfo.displayName().value().trimmed();

        if (displayName.isEmpty()) {
            sendError(connection.sessionId(), "Display name must not be empty");
            return;
        }

        updateInfo.setDisplayName(displayName);
    }

    if (updateInfo.email().has_value()) {
        const QString email = updateInfo.email().value().trimmed();

        if (email.isEmpty()) {
            sendError(connection.sessionId(), "Email must not be empty");
            return;
        }

        updateInfo.setEmail(email);
    }

    if (updateInfo.passwordHash().has_value() && updateInfo.passwordHash().value().isEmpty()) {
        sendError(connection.sessionId(), "Password hash must not be empty");
        return;
    }

    const Database& readDb = Database::instance();
    const QUuid userId = connection.userId().value();

    if (updateInfo.username().has_value()) {
        const auto existingUser = readDb.getUserByUsername(updateInfo.username().value());
        if (existingUser.has_value() && existingUser->id() != userId) {
            sendError(connection.sessionId(), "Username already exists");
            return;
        }
    }

    if (updateInfo.email().has_value()) {
        const auto existingUser = readDb.getUserByEmail(updateInfo.email().value());
        if (existingUser.has_value() && existingUser->id() != userId) {
            sendError(connection.sessionId(), "Email already exists");
            return;
        }
    }

    Database& db = Database::instance();
    const auto updatedUser = db.updateUserProfile(userId, updateInfo);

    if (!updatedUser.has_value()) {
        sendError(connection.sessionId(), "Failed to update profile");
        return;
    }

    const auto updatedProfileInfo = db.getProfileInfoByUserId(userId);
    if (!updatedProfileInfo.has_value()) {
        sendError(connection.sessionId(), "Failed to read updated profile");
        return;
    }

    sendSuccess(connection.sessionId(), "Profile updated successfully");
    sendUserProfileData(connection.sessionId(), updatedProfileInfo.value());
}

void Server::handleGetUserInfo(const QTcpSocket* socket, const shared::Packet& packet)
{
    if (!socket) return;
    if (packet.type() != shared::PacketType::GET_USER_INFO) return;

    const auto connectionOpt = findConnection(packet.sender());
    if (!connectionOpt.has_value())
    {
        qWarning() << "Client" << packet.sender() << "not yet connected";
        return;
    }

    const ClientConnection& connection = connectionOpt->get();

    if (!connection.matchesSocket(socket) || !connection.isAuthorized())
    {
        sendError(connection.sessionId(), "Not authorized");
        return;
    }

    if (!packet.hasPayload())
    {
        sendError(connection.sessionId(), "User info request payload is missing");
        return;
    }

    const auto payload = decryptPacketPayload(packet);
    if (!payload.has_value())
    {
        sendError(connection.sessionId(), "Unable to decrypt user info payload");
        return;
    }

    const auto requestOpt = shared::UserInfoRequest::deserialize(payload.value());
    if (!requestOpt.has_value())
    {
        sendError(connection.sessionId(), "Invalid user info request payload");
        return;
    }

    const Database& db = Database::instance();
    std::optional<shared::PublicUserInfo> publicUserInfo;

    switch (requestOpt->identifierType())
    {
    case shared::UserIdentifierType::UUID:
        if (requestOpt->userId().isNull())
        {
            sendError(connection.sessionId(), "Invalid user id");
            return;
        }

        publicUserInfo = db.getPublicUserInfoByUserId(requestOpt->userId());
        break;

    case shared::UserIdentifierType::USERNAME:
        {
            const QString username = shared::util::normalizeUsername(requestOpt->username());

            if (!shared::util::isValidUsername(username))
            {
                sendError(connection.sessionId(), "Invalid username");
                return;
            }

            publicUserInfo = db.getPublicUserInfoByUsername(username);
        }
        break;
    }

    if (!publicUserInfo.has_value())
    {
        sendError(connection.sessionId(), "User not found");
        return;
    }

    sendPublicUserInfoData(connection.sessionId(), publicUserInfo.value());
}

void Server::handleGetUserSession(const QTcpSocket* socket, const shared::Packet& packet)
{
    if (!socket) return;
    if (packet.type() != shared::PacketType::GET_USER_SESSION) return;

    const auto connectionOpt = findConnection(packet.sender());
    if (!connectionOpt.has_value())
        return;

    const ClientConnection& connection = connectionOpt->get();
    if (!connection.matchesSocket(socket) || !connection.isAuthorized())
    {
        sendError(connection.sessionId(), "Not authorized");
        return;
    }

    if (!packet.hasPayload())
    {
        sendError(connection.sessionId(), "User session request payload is missing");
        return;
    }

    const auto payload = decryptPacketPayload(packet);
    if (!payload.has_value() || payload->size() != 16)
    {
        sendError(connection.sessionId(), "Invalid user session request payload");
        return;
    }

    const QUuid requestedUserId = QUuid::fromRfc4122(payload.value());
    QUuid requestedSessionId;

    for (auto it = m_clients.constBegin(); it != m_clients.constEnd(); ++it)
    {
        const ClientConnection& client = it.value();
        if (!client.isAuthorized() || !client.userId().has_value())
            continue;

        if (client.userId().value() == requestedUserId)
        {
            requestedSessionId = client.sessionId();
            break;
        }
    }

    sendUserSessionData(connection.sessionId(), shared::SessionInfo(requestedUserId, requestedSessionId));
}

void Server::handleChatMessage(const ClientConnection& connection, const shared::Packet& packet) const
{
    if (!packet.hasPayload())
    {
        sendError(connection.sessionId(), "Message payload is missing");
        return;
    }

    auto messageBytes = decryptPacketPayload(packet);
    if (!messageBytes.has_value())
    {
        sendError(connection.sessionId(), "Unable to decrypt message payload");
        return;
    }

    const shared::Message incomingMessage = shared::Message::deserialize(messageBytes.value());
    const QUuid chatId = incomingMessage.targetChatId();

    if (chatId.isNull())
    {
        qWarning() << "Declining message from" << connection.sessionId().toString() << "with invalid chat UUID";
        sendError(connection.sessionId(), "Invalid chat id");
        return;
    }

    if (!connection.userId().has_value())
    {
        qWarning() << "Declining message from client without account id";
        sendError(connection.sessionId(), "Not authorized");
        return;
    }

    const Database& db = Database::instance();
    const QList<QUuid> memberUserIds = db.getUserIdsByChatId(chatId);

    if (memberUserIds.isEmpty())
    {
        qWarning() << "Chat" << chatId.toString() << "not found or has no members";
        sendError(connection.sessionId(), "Chat not found");
        return;
    }

    if (incomingMessage.type() == shared::MessageType::INVALID)
    {
        qWarning() << "Declining invalid message from" << connection.sessionId().toString();
        sendError(connection.sessionId(), "Invalid message payload");
        return;
    }

    const QUuid authenticatedUserId = connection.userId().value();
    const QUuid senderUserId = incomingMessage.senderUserId();

    if (senderUserId != authenticatedUserId)
    {
        qWarning() << "Client" << connection.sessionId().toString()
                   << "tried to send message as user" << senderUserId.toString();
        sendError(connection.sessionId(), "Invalid sender user id");
        return;
    }

    if (!memberUserIds.contains(authenticatedUserId))
    {
        qWarning() << "User" << authenticatedUserId.toString()
                   << "is not a member of chat" << chatId.toString();
        sendError(connection.sessionId(), "You are not a member of this chat");
        return;
    }

    qInfo() << "Routing message from session" << connection.sessionId().toString()
            << "user" << senderUserId.toString()
            << "to chat" << chatId.toString();

    for (auto it = m_clients.constBegin(); it != m_clients.constEnd(); ++it)
    {
        const ClientConnection& targetConnection = it.value();

        if (!targetConnection.isAuthorized() || !targetConnection.userId().has_value())
            continue;

        if (!memberUserIds.contains(targetConnection.userId().value()))
            continue;

        if (it.key() == connection.sessionId())
            continue;

        const auto encryptionKey = peerEncryptionKey(it.key());
        if (!encryptionKey.has_value())
            continue;

        const shared::Packet outboundPacket = shared::PacketFactory::chatMessagePacket(
            m_uuid,
            it.key(),
            incomingMessage,
            encryptionKey.value()
        );

        sendPacket(it.key(), outboundPacket);
    }
}

void Server::sendPublicKey(const QUuid& receiverSessionId) const
{
    const auto derivedKeyPair = shared::crypto::deriveKeyPair(receiverSessionId);

    const auto packet = shared::PacketFactory::publicKeyExchangePacket(
        m_uuid,
        receiverSessionId,
        shared::KeyStore::instance().publicKey(),
        derivedKeyPair.first
    );

    sendPacket(receiverSessionId, packet);
}

void Server::handleKeyExchange(const QTcpSocket* socket, const shared::Packet& packet)
{
    if (!socket) return;
    if (packet.type() != shared::PacketType::PUBLIC_KEY_EXCHANGE) return;

    const auto connectionOpt = findConnection(packet.sender());
    if (!connectionOpt.has_value())
    {
        qWarning() << "Client" << packet.sender() << "not yet connected, cannot exchange keys";
        return;
    }

    const ClientConnection& connection = connectionOpt->get();

    if (!connection.matchesSocket(socket))
        return;

    if (!packet.hasPayload())
    {
        sendError(connection.sessionId(), "Public key payload is missing");
        return;
    }

    const QByteArray decryptionKey = shared::crypto::deriveKeyPair(packet.receiver()).second;

    const auto payload = packet.payload(decryptionKey);
    if (!payload.has_value() || payload->isEmpty())
    {
        sendError(connection.sessionId(), "Unable to decrypt public key payload");
        return;
    }

    const bool alreadyHadPeerKey = shared::KeyStore::instance().hasPeerPublicKey(connection.sessionId());
    QByteArray clientPublicKey = payload.value();
    shared::KeyStore::instance().setPeerPublicKey(connection.sessionId(), std::move(clientPublicKey));

    if (!alreadyHadPeerKey)
        sendPublicKey(connection.sessionId());

    sendSuccess(connection.sessionId(), "Public key registered");
}
