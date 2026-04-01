#include "TcpServer.h"

#include <QByteArray>
#include <QDebug>

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
void TcpServer::sendPacket(const QUuid& target, const shared::Packet& packet) const
{
	QTcpSocket* socket = m_sockets[target];
	QByteArray payload;
	payload.append(packet.serialize());
	payload.append(DELIMITER);

	qInfo() << "Sending a packet to" << target.toString();

	if (socket->write(payload) == -1)
		qCritical() << "Error writing to" << target.toString() << ":" << socket->errorString();
}

void TcpServer::broadcast(const QString& text) const
{
	const QByteArray msgBytes = shared::Message(shared::MessageType::TEXT, text).serialize();

	for (const auto uuid : m_sockets.keys())
	{
		const shared::Packet packet(shared::PacketType::MESSAGE, m_uuid, uuid, msgBytes);
		sendPacket(uuid, packet);
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
			if (packet.type() == shared::PacketType::CONNECT)
			{
				registerClient(socket, packet);
				continue;
			}

        	handlePacket(packet);
		}
    }
}

/**
 * @brief Handles client disconnection
 */
void TcpServer::onClientDisconnected()
{
	auto* socket = qobject_cast<QTcpSocket*>(sender());
	if (!socket) return;

	for (const auto& uuid : m_sockets.keys())
	{
		if (m_sockets.contains(uuid) && m_sockets.value(uuid) == socket)
		{
			m_sockets.remove(uuid);
			qInfo() << "Client" << uuid.toString() << "disconnected";
		}
	}

	socket->close();
	socket->deleteLater();
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

			for (const auto& uuid : m_sockets.keys())
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

/**
 * @brief Registers a new client after receiving a connect packet.
 * Adds the client to the internal socket map and sends a welcome message.
 * @param socket The client's QTcpSocket.
 * @param packet Connect packet containing the client's UUID.
 */
void TcpServer::registerClient(QTcpSocket* socket, const shared::Packet& packet)
{
	if (!socket) return;
	if (packet.type() != shared::PacketType::CONNECT) return;

	const QUuid uuid = packet.sender();

	if (m_sockets.contains(uuid))
	{
		qWarning() << "Client already registered";
		return;
	}

	m_sockets.insert(uuid, socket);

	qInfo() << "New client" << uuid.toString() << "connected";
	const auto welcomeMessage = shared::PacketFactory::textMessagePacket(m_uuid, uuid, "Hello, client!");
	sendPacket(uuid, welcomeMessage);
}
