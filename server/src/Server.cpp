#include "Server.h"

#include <QByteArray>
#include <QDebug>
#include <qlogging.h>

#include "dto/AuthInfo.h"
#include "Message.h"
#include "PacketFactory.h"
#include "dto/ProfileInfo.h"
#include "dto/ProfileUpdateInfo.h"
#include "dto/PublicUserInfo.h"

#include "util.h"

/**
 * @brief Returns the singleton Server instance
 */
Server& Server::instance()
{
	static Server instance;
	return instance;
}

/**
 * @brief Constructs the Server object
 * Sets up TCP server, console reader, and signal connections.
 */
Server::Server(QObject* parent)
	: QObject(parent),
	  m_server(new QTcpServer(this)),

	  m_isRunning(false)
{
	connect(m_server, &QTcpServer::newConnection, this, &Server::onNewConnection);

}

Server::~Server()
{
	stop();
}

/**
 * @brief Starts the TCP server
 * @param port Port number to listen on
 * @return true if server started successfully
 */
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

/**
 * @brief Stops the server and closes the connection
 */
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

void Server::sendError(const QUuid& receiverSessionId, QString message) const
{
	auto packet = shared::PacketFactory::errorPacket(m_uuid, receiverSessionId, std::move(message));
	sendPacket(receiverSessionId, packet);
}

void Server::sendSuccess(const QUuid& receiverSessionId, QString message) const
{
	auto packet = shared::PacketFactory::successPacket(m_uuid, receiverSessionId, std::move(message));
	sendPacket(receiverSessionId, packet);
}

void Server::sendProfileData(const QUuid& receiverSessionId, const shared::ProfileInfo& info) const
{
    const auto packet = shared::PacketFactory::profileDataPacket(m_uuid, receiverSessionId, info);
    sendPacket(receiverSessionId, packet);
}

void Server::sendUserInfoData(const QUuid& receiverSessionId, const shared::PublicUserInfo& info) const
{
    const auto packet = shared::PacketFactory::userInfoDataPacket(m_uuid, receiverSessionId, info);
    sendPacket(receiverSessionId, packet);
}

/**
 * Handles new incoming TCP connections; returns None if socket is not defined
 */
void Server::onNewConnection()
{
	const QTcpSocket* socket = m_server->nextPendingConnection();
	if (!socket) return;

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
        const QList<shared::Packet> packets = shared::util::parse(bytes);

        for (const auto& packet : packets)
        {
            switch (packet.type())
            {
            case shared::PacketType::CONNECT:
                handleConnect(socket, packet);
                break;

            case shared::PacketType::REGISTER:
                handleRegister(socket, packet);
                break;

            case shared::PacketType::LOGIN:
                handleLogin(socket, packet);
                break;

            case shared::PacketType::LOGOUT:
                handleLogout(socket, packet);
                break;

            case shared::PacketType::PROFILE_REQUEST:
                handleProfileRequest(socket, packet);
                break;

            case shared::PacketType::PROFILE_UPDATE:
                handleProfileUpdate(socket, packet);
                break;

            case shared::PacketType::USER_INFO_REQUEST:
                handleUserInfoRequest(socket, packet);
                break;

            default:
                handleAuthorizedPacket(packet);
                break;
            }

		}
    }
}

/**
 * @brief Handles client disconnection
 */
void Server::onClientDisconnected()
{
    auto* clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket)
        return;

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

/**
 * @brief Connects a new client after receiving a connect packet.
 * Adds the client to the internal socket map and sends a welcome message.
 * @param socket The client's QTcpSocket.
 * @param packet Connect packet containing the client's UUID.
 */
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

	sendSuccess(sessionId, "Connected");
    qInfo() << "Connected a new client" << sessionId;
}

void Server::handleRegister(const QTcpSocket* socket, const shared::Packet& packet)
{
    if (!socket) return;
    if (packet.type() != shared::PacketType::REGISTER) return;

    const auto connectionOpt = findConnection(packet.sender());

    if (!connectionOpt.has_value())
    {
        qWarning() << "Client" << packet.sender() << "not yet connected, cannot register unconnected client";
        return;
    }

    const ClientConnection& connection = connectionOpt->get();

    if (!connection.matchesSocket(socket) || !packet.data().has_value()) return;

    const auto registerInfo = shared::RegisterInfo::deserialize(packet.data().value());

    if (!registerInfo.has_value())
    {
        qWarning() << "Client" << packet.sender() << "sent invalid register payload";
        sendPacket(connection.sessionId(), shared::PacketFactory::errorPacket(m_uuid, connection.sessionId(), "Invalid register payload"));
        return;
    }

    Database& db = Database::instance();

    if (db.getUserByUsername(registerInfo->username()).has_value())
    {
        qWarning() << "Username" << registerInfo->username() << "already exists";
        sendPacket(connection.sessionId(), shared::PacketFactory::errorPacket(m_uuid, connection.sessionId(), "Username already exists"));
        return;
    }

    const model::User user(
        registerInfo->username(),
        registerInfo->name(),
        registerInfo->passwordHash(),
        registerInfo->email()
    );

    if (!db.createUser(user))
    {
        qCritical() << "Registration of client" << packet.sender() << "failed";
        sendPacket(connection.sessionId(), shared::PacketFactory::errorPacket(m_uuid, connection.sessionId(), "Registration failed"));
        return;
    }

    sendSuccess(connection.sessionId(), "Registration successful");
    qInfo() << "Successfully registered client" << packet.sender() << "as" << registerInfo->username();
}

void Server::handleLogin(const QTcpSocket* socket, const shared::Packet& packet)
{
    if (!socket) return;
    if (packet.type() != shared::PacketType::LOGIN) return;

    const auto connectionOpt = findConnection(packet.sender());

    if (!connectionOpt.has_value())
    {
        qWarning() << "Client" << packet.sender() << "not yet connected, cannot log in unconnected client";
        return;
    }

    ClientConnection& connection = connectionOpt->get();

    if (!connection.matchesSocket(socket) || !packet.data().has_value()) return;

    const auto loginInfo = shared::LoginInfo::deserialize(packet.data().value());

    if (!loginInfo.has_value())
    {
        qWarning() << "Client" << packet.sender() << "sent invalid login payload";
        sendPacket(connection.sessionId(), shared::PacketFactory::errorPacket(m_uuid, connection.sessionId(), "Invalid login payload"));
        return;
    }

    const Database& db = Database::instance();
    const auto user = db.authenticateUser(loginInfo.value());

    if (!user.has_value())
    {
        sendPacket(connection.sessionId(), shared::PacketFactory::errorPacket(m_uuid, connection.sessionId(), "Invalid login or password"));
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
    case shared::PacketType::MESSAGE:
        handleChatMessage(connection, packet);
        break;

    case shared::PacketType::COMMAND:
        qInfo() << "Command from" << sessionId.toString();
        break;

    default:
        qInfo() << "Unknown or unsupported message from" << sessionId.toString();
        break;
    }
}

void Server::handleLogout(const QTcpSocket* socket, const shared::Packet& packet)
{
    if (!socket) return;
    if (packet.type() != shared::PacketType::LOGOUT) return;

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

void Server::handleProfileRequest(const QTcpSocket* socket, const shared::Packet& packet)
{
    if (!socket) return;
    if (packet.type() != shared::PacketType::PROFILE_REQUEST) return;

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

    sendProfileData(connection.sessionId(), profileInfo.value());
}

void Server::handleProfileUpdate(const QTcpSocket* socket, const shared::Packet& packet)
{
    if (!socket) return;
    if (packet.type() != shared::PacketType::PROFILE_UPDATE) return;

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

    if (!packet.data().has_value()) {
        sendError(connection.sessionId(), "Profile update payload is missing");
        return;
    }

    const auto updateInfoOpt = shared::ProfileUpdateInfo::deserialize(packet.data().value());
    if (!updateInfoOpt.has_value()) {
        sendError(connection.sessionId(), "Invalid profile update payload");
        return;
    }

    shared::ProfileUpdateInfo updateInfo = updateInfoOpt.value();

    if (updateInfo.username().has_value()) {
        const QString username = updateInfo.username().value().trimmed();

        if (username.isEmpty()) {
            sendError(connection.sessionId(), "Username must not be empty");
            return;
        }

        updateInfo.setUsername(username);
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
    sendProfileData(connection.sessionId(), updatedProfileInfo.value());
}

void Server::handleUserInfoRequest(const QTcpSocket* socket, const shared::Packet& packet)
{
    if (!socket) return;
    if (packet.type() != shared::PacketType::USER_INFO_REQUEST) return;

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

    if (!packet.data().has_value()) {
        sendError(connection.sessionId(), "User info request payload is missing");
        return;
    }

    const QUuid requestedUserId = QUuid::fromRfc4122(packet.data().value());

    if (requestedUserId.isNull()) {
        sendError(connection.sessionId(), "Invalid user id");
        return;
    }

    const Database& db = Database::instance();
    const auto publicUserInfo = db.getPublicUserInfoByUserId(requestedUserId);

    if (!publicUserInfo.has_value()) {
        sendError(connection.sessionId(), "User not found");
        return;
    }

    sendUserInfoData(connection.sessionId(), publicUserInfo.value());
}

void Server::handleChatMessage(const ClientConnection& connection, const shared::Packet& packet) const
{
    if (!packet.data().has_value())
    {
        sendError(connection.sessionId(), "Message payload is missing");
        return;
    }

    const shared::Message incomingMessage = shared::Message::deserialize(packet.data().value());

    const QUuid chatId = !incomingMessage.targetChatId().isNull()
        ? incomingMessage.targetChatId()
        : packet.receiver();

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

    const QUuid senderUserId = connection.userId().value();

    if (!memberUserIds.contains(senderUserId))
    {
        qWarning() << "User" << senderUserId.toString()
                   << "is not a member of chat" << chatId.toString();
        sendError(connection.sessionId(), "You are not a member of this chat");
        return;
    }

    shared::Message normalizedMessage(
        senderUserId,
        chatId,
        incomingMessage.type(),
        incomingMessage.content()
    );

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

        const auto outboundPacket = shared::PacketFactory::messagePacket(
            connection.sessionId(),
            it.key(),
            std::move(normalizedMessage)
        );

        sendPacket(it.key(), outboundPacket);
    }
}

