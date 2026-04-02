#include "TcpServer.h"

#include <QByteArray>
#include <QDebug>

#include "AuthInfo.h"
#include "Database.h"
#include "Message.h"
#include "PacketFactory.h"
#include "util.h"

/**
 * @brief Returns the singleton TcpServer instance
 */
TcpServer& TcpServer::instance()
{
	static TcpServer instance;
	return instance;
}

/**
 * @brief Constructs the TcpServer object
 * Sets up TCP server, console reader, and signal connections.
 */
TcpServer::TcpServer(QObject* parent)
	: QObject(parent),
	  m_server(new QTcpServer(this)),
	  m_consoleReader(new ConsoleReader(this)),
	  m_isRunning(false)
{
	connect(m_server, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);
	connect(m_consoleReader, &ConsoleReader::lineRead, this, [this](const QString& line) {
		if (!line.isEmpty())
			broadcast(line);
	}, Qt::QueuedConnection);
}
UserConnection* TcpServer::findConnection(const QUuid& sessionId)
{
    auto it = m_users.find(sessionId);
    return it == m_users.end() ? nullptr : &it.value();
}

UserConnection* TcpServer::findConnection(QTcpSocket* socket)
{
    for (auto it = m_users.begin(); it != m_users.end(); ++it) {
        if (it.value().socket() == socket)
            return &it.value();
    }
    return nullptr;
}
/**
 * @brief Registers a new client after receiving a connect packet.
 * Adds the client to the internal socket map and sends a welcome message.
 * @param socket The client's QTcpSocket.
 * @param packet Connect packet containing the client's UUID.
 */
void TcpServer::registerClient(QTcpSocket* socket, const shared::Packet& packet)
{
    if (!socket || packet.type() != shared::PacketType::CONNECT || packet.sender().isNull())
        return;

    const QUuid sessionId = packet.sender();
    if (m_users.contains(sessionId)) {
        qWarning() << "Client already registered";
        return;
    }

    m_users.insert(sessionId, UserConnection(sessionId, socket));

    const auto ok = shared::PacketFactory::successPacket(m_uuid, sessionId, "Connected");
    sendPacket(sessionId, ok);
}
void TcpServer::handleRegister(QTcpSocket* socket, const shared::Packet& packet)
{
    auto* connection = findConnection(packet.sender());
    if (!connection || connection->socket() != socket || !packet.data().has_value()) {
        return;
    }

    const auto info = shared::RegisterInfo::deserialize(packet.data().value());
    if (!info.has_value()) {
        sendPacket(connection->sessionId(),
                   shared::PacketFactory::errorPacket(m_uuid, connection->sessionId(), "Invalid register payload"));
        return;
    }

    Database& db = Database::instance();

    if (db.getUserByUsername(info->username()).has_value()) {
        sendPacket(connection->sessionId(),
                   shared::PacketFactory::errorPacket(m_uuid, connection->sessionId(), "Username already exists"));
        return;
    }

    model::User user;
    user.setId(QUuid::createUuid());
    user.setUsername(info->username());
    user.setName(info->name());
    user.setPasswordHash(info->passwordHash());
    user.setEmail(info->email());

    if (!db.createUser(user)) {
        sendPacket(connection->sessionId(),
                   shared::PacketFactory::errorPacket(m_uuid, connection->sessionId(), "Registration failed"));
        return;
    }

    sendPacket(connection->sessionId(),
               shared::PacketFactory::successPacket(m_uuid, connection->sessionId(), "Registration successful"));
}
void TcpServer::handleLogin(QTcpSocket* socket, const shared::Packet& packet)
{
    auto* connection = findConnection(packet.sender());
    if (!connection || connection->socket() != socket || !packet.data().has_value()) {
        return;
    }

    const auto info = shared::LoginInfo::deserialize(packet.data().value());
    if (!info.has_value()) {
        sendPacket(connection->sessionId(),
                   shared::PacketFactory::errorPacket(m_uuid, connection->sessionId(), "Invalid login payload"));
        return;
    }

    Database& db = Database::instance();
    const auto user = db.authenticateUser(info.value());

    if (!user.has_value()) {
        sendPacket(connection->sessionId(),
                   shared::PacketFactory::errorPacket(m_uuid, connection->sessionId(), "Invalid login or password"));
        return;
    }

    connection->authorize(user->id());

    sendPacket(connection->sessionId(),
               shared::PacketFactory::successPacket(m_uuid, connection->sessionId(), "Login successful"));
}


TcpServer::~TcpServer()
{
	stop();
}

/**
 * @brief Starts the TCP server
 * @param port Port number to listen on
 * @return true if server started successfully
 */
bool TcpServer::start(const uint16_t port)
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
 * @brief Constant function which stops the server and closes the connection
 */
void TcpServer::stop() const
{
	if (m_isRunning)
		m_server->close();

	m_consoleReader->stop();
}

/**
 * @brief Sends a packet to a specific client using bytes stream
 */
void TcpServer::sendPacket(const QUuid& targetSessionId, const shared::Packet& packet) const
{
    auto it = m_users.constFind(targetSessionId);
    if (it == m_users.constEnd()) {
        qWarning() << "Target session not found:" << targetSessionId.toString();
        return;
    }

    QTcpSocket* clientSocket = it.value().socket();
    if (!clientSocket) {
        qWarning() << "Target socket is null:" << targetSessionId.toString();
        return;
    }

    QByteArray payload;
    payload.append(packet.serialize());
    payload.append(DELIMITER);

    qInfo() << "Sending a packet to" << targetSessionId.toString();

    if (clientSocket->write(payload) == -1)
        qCritical() << "Error writing to" << targetSessionId.toString() << ":" << clientSocket->errorString();
}


void TcpServer::broadcast(const QString& text) const
{
    const QByteArray messageBytes = shared::Message(shared::MessageType::TEXT, text).serialize();

    for (const QUuid& targetSessionId : m_users.keys())
    {
        const shared::Packet messagePacket(
            shared::PacketType::MESSAGE,
            m_uuid,
            targetSessionId,
            messageBytes
            );

        sendPacket(targetSessionId, messagePacket);
    }
}

/**
 * Handles new incoming TCP connections; returns None if socket is not defined
 */
void TcpServer::onNewConnection()
{
	const QTcpSocket* socket = m_server->nextPendingConnection();
	if (!socket) return;

	connect(socket, &QTcpSocket::readyRead, this, &TcpServer::onServerRead);
	connect(socket, &QTcpSocket::disconnected, this, &TcpServer::onClientDisconnected);
}

void TcpServer::onServerRead()
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
                registerClient(socket, packet);
                break;
            case shared::PacketType::REGISTER:
                handleRegister(socket, packet);
                break;
            case shared::PacketType::LOGIN:
                handleLogin(socket, packet);
                break;
            default:
                handlePacket(packet);
                break;
            }

		}
    }
}

/**
 * @brief Handles client disconnection
 */
void TcpServer::onClientDisconnected()
{
    auto* clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket)
        return;

    for (auto it = m_users.begin(); it != m_users.end(); ++it)
    {
        if (it.value().socket() == clientSocket)
        {
            const QUuid disconnectedSessionId = it.key();
            m_users.erase(it);
            qInfo() << "Client session" << disconnectedSessionId.toString() << "disconnected";
            break;
        }
    }

    clientSocket->close();
    clientSocket->deleteLater();
}

/**
 * @brief Handles client disconnection.
 *
 * Removes the client from the socket map, logs the event, and deletes the socket.
 */
void TcpServer::handlePacket(const shared::Packet& packet) const
{
	if (packet.sender().isNull())
	{
		qWarning() << "Ignoring message with invalid UUID";
		return;
	}

	switch (packet.type())
	{
	case shared::PacketType::MESSAGE:
		{
			qInfo() << "Received message from" << packet.sender().toString();

            for (const auto& uuid : m_users.keys())
			{
				if (uuid == packet.sender()) continue;
				sendPacket(uuid, packet);
			}
		}
		break;
	case shared::PacketType::COMMAND:
		{
			qInfo() << "Command from" << packet.sender().toString();
		}
		break;
	default:
		{
			qInfo() << "Unknown or unsupported message from" << packet.sender().toString();
		}
		break;
	}
}



