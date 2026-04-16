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
	  m_consoleReader(new ConsoleReader(this)),
	  m_isRunning(false)
{
	connect(m_server, &QTcpServer::newConnection, this, &Server::onNewConnection);
	connect(m_consoleReader, &ConsoleReader::lineRead, this, [this](const QString& line) {
		if (!line.isEmpty())
			broadcast(line);
	}, Qt::QueuedConnection);
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
	m_consoleReader->start();

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

	m_consoleReader->stop();
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

void Server::sendError(const QUuid& targetSessionId, QString message) const
{
	auto packet = shared::PacketFactory::errorPacket(m_uuid, targetSessionId, std::move(message));
	sendPacket(targetSessionId, packet);
}

void Server::sendSuccess(const QUuid& targetSessionId, QString message) const
{
	auto packet = shared::PacketFactory::successPacket(m_uuid, targetSessionId, std::move(message));
	sendPacket(targetSessionId, packet);
}

void Server::broadcast(const QString& text) const
{
    for (const QUuid& targetSessionId : m_clients.keys())
    {
        const shared::Packet packet =
            shared::PacketFactory::textMessagePacket(m_uuid, targetSessionId, text);

        sendPacket(targetSessionId, packet);
    }
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
        if (it.value().matchesSocket(clientSocket))
        {
            const QUuid disconnectedSessionId = it.key();
            m_clients.erase(it);
            qInfo() << "Client session" << disconnectedSessionId.toString() << "disconnected";
            break;
        }
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
		qInfo() << "Received message from" << uuid.toString();
		for (const auto& clientUuid : m_clients.keys())
		{
			if (uuid == clientUuid) continue;
			sendPacket(clientUuid, packet);
		}
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

