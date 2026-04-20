#include "Server.h"

#include <QByteArray>
#include <QDebug>
#include <qlogging.h>

#include "AuthInfo.h"
#include "Database.h"
#include "PacketFactory.h"
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
void Server::sendPacket(const QUuid& targetSessionId, const shared::Packet& packet) const
{
    auto it = m_clients.constFind(targetSessionId);
    if (it == m_clients.constEnd()) {
        qWarning() << "Target session not found:" << targetSessionId.toString();
        return;
    }

    if (!it.value().sendPacket(packet)) {
        qCritical() << "Error writing to" << targetSessionId.toString();
    }
}

/**
 * @brief Sends an error packet to a specific client
 * @param targetSessionId The session UUID of the target client
 * @param message The error message to send
 */
void Server::sendError(const QUuid& targetSessionId, QString message) const
{
	auto packet = shared::PacketFactory::errorPacket(m_uuid, targetSessionId, std::move(message));
	sendPacket(targetSessionId, packet);
}

/**
 * @brief Sends a success packet to a specific client
 * @param targetSessionId The session UUID of the target client
 * @param message The success message to send
 */
void Server::sendSuccess(const QUuid& targetSessionId, QString message) const
{
	auto packet = shared::PacketFactory::successPacket(m_uuid, targetSessionId, std::move(message));
	sendPacket(targetSessionId, packet);
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

/**
 * @brief Handles incoming data from connected clients
 * Reads all available data, parses it into packets, and dispatches to appropriate handlers
 */
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

/**
 * @brief Finds a client connection by session UUID (non-const version)
 * @param sessionId The session UUID to search for
 * @return An optional reference wrapper to the ClientConnection if found, std::nullopt otherwise
 */
std::optional<std::reference_wrapper<ClientConnection>> Server::findConnection(const QUuid& sessionId)
{
    const auto it = m_clients.find(sessionId);

    if (it == m_clients.end())
        return std::nullopt;

    return it.value();
}

/**
 * @brief Finds a client connection by session UUID (const version)
 * @param sessionId The session UUID to search for
 * @return An optional const reference wrapper to the ClientConnection if found, std::nullopt otherwise
 */
std::optional<std::reference_wrapper<const ClientConnection>> Server::findConnection(const QUuid& sessionId) const
{
    const auto it = m_clients.find(sessionId);

    if (it == m_clients.end())
        return std::nullopt;

    return it.value();
}

/**
 * @brief Finds a client connection by socket pointer (non-const version)
 * @param clientSocket The QTcpSocket pointer to search for
 * @return An optional reference wrapper to the ClientConnection if found, std::nullopt otherwise
 */
std::optional<std::reference_wrapper<ClientConnection>> Server::findConnection(const QTcpSocket* clientSocket)
{
    for (auto it = m_clients.begin(); it != m_clients.end(); ++it)
    {
        if (it.value().matchesSocket(clientSocket))
            return it.value();
    }

    return std::nullopt;
}

/**
 * @brief Finds a client connection by socket pointer (const version)
 * @param clientSocket The QTcpSocket pointer to search for
 * @return An optional const reference wrapper to the ClientConnection if found, std::nullopt otherwise
 */
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

/**
 * @brief Handles user registration requests
 * Validates the registration data, checks for existing username, and creates a new user account
 * @param socket The client's QTcpSocket
 * @param packet Registration packet containing user data
 */
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

/**
 * @brief Handles user login requests
 * Authenticates the user credentials and authorizes the client connection
 * @param socket The client's QTcpSocket
 * @param packet Login packet containing authentication data
 */
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

/**
 * @brief Handles packets that require authorization
 * Routes authorized packets to appropriate handlers based on packet type
 * @param packet The packet to handle
 */
void Server::handleAuthorizedPacket(const shared::Packet& packet) const
{
	const QUuid& uuid = packet.sender();

	if (uuid.isNull())
	{
		qWarning() << "Ignoring packet with invalid UUID";
		return;
	}

	auto connectionOpt = findConnection(uuid);
	if (!connectionOpt)
	{
		qWarning() << "Ignoring packet from unconnected client";
		return;
	}

	const ClientConnection& connection = connectionOpt.value().get();

	if (!connection.isAuthorized())
	{
		qWarning() << "Declining a packet from unauthorized client";
		sendError(uuid, "Not authorized");
		return;
	}

	switch (packet.type())
	{
    case shared::PacketType::MESSAGE:
        handleChatMessage(connection, packet);
        break;


	case shared::PacketType::COMMAND:
		qInfo() << "Command from" << uuid.toString();
		break;

	default:
		qInfo() << "Unknown or unsupported message from" << uuid.toString();
		break;

	}

	sendSuccess(uuid, "Packet successfully recieved");
}

/**
 * @brief Handles user logout requests
 * Logs out the user from their current session
 * @param socket The client's QTcpSocket
 * @param packet Logout packet
 */
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

/**
 * @brief Handles chat message routing
 * Validates the sender's membership in the chat and forwards the message to all other chat members
 * @param connection The sender's client connection
 * @param packet The message packet to route
 */
void Server::handleChatMessage(const ClientConnection& connection, const shared::Packet& packet) const
{
    const QUuid& sessionId = packet.sender();
    const QUuid& chatId = packet.target();

    if (chatId.isNull())
    {
        qWarning() << "Declining message from" << sessionId.toString() << "with invalid chat UUID";
        sendError(sessionId, "Invalid chat id");
        return;
    }

    if (!connection.userId().has_value())
    {
        qWarning() << "Declining message from client without account id";
        sendError(sessionId, "Not authorized");
        return;
    }

    const Database& db = Database::instance();
    const QList<QUuid> memberUserIds = db.getUserIdsByChatId(chatId);

    if (memberUserIds.isEmpty())
    {
        qWarning() << "Chat" << chatId.toString() << "not found or has no members";
        sendError(sessionId, "Chat not found");
        return;
    }

    const QUuid senderUserId = connection.userId().value();

    if (!memberUserIds.contains(senderUserId))
    {
        qWarning() << "User" << senderUserId.toString()
        << "is not a member of chat" << chatId.toString();
        sendError(sessionId, "You are not a member of this chat");
        return;
    }

    qInfo() << "Routing message from session" << sessionId.toString()
            << "to chat" << chatId.toString();

    for (auto it = m_clients.constBegin(); it != m_clients.constEnd(); ++it)
    {
        const ClientConnection& targetConnection = it.value();

        if (!targetConnection.isAuthorized() || !targetConnection.userId().has_value())
            continue;

        if (!memberUserIds.contains(targetConnection.userId().value()))
            continue;

        if (it.key() == sessionId)
            continue;

        sendPacket(it.key(), packet);
    }

    sendSuccess(sessionId, "Packet successfully received");
}