#include "TcpServer.h"

#include <QByteArray>
#include <QDebug>

#include "util.h"

TcpServer& TcpServer::instance()
{
	static TcpServer instance;
	return instance;
}

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

void TcpServer::stop() const
{
	if (m_isRunning)
		m_server->close();

	m_consoleReader->stop();
}

void TcpServer::sendMessage(const QUuid& target, const shared::Message& msg) const
{
	QTcpSocket* socket = m_sockets[target];
	QByteArray bytes;
	bytes.append(msg.encode());
	bytes.append('\x01');

	qInfo() << "Sending" << msg.content() << "to" << target.toString();

	if (socket->write(bytes) == -1)
		qCritical() << "Error writing to" << target.toString() << ":" << socket->errorString();
}

void TcpServer::broadcast(const QString& text) const
{
	const shared::Message msg(shared::MessageType::Text, m_uuid, text);

	for (const auto uuid : m_sockets.keys())
	{
		sendMessage(uuid, msg);
	}
}

void TcpServer::onNewConnection()
{
	QTcpSocket* socket = m_server->nextPendingConnection();
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
        const QList<shared::Message> messages = shared::util::parse(bytes);

        for (const auto& msg : messages)
        {
			if (msg.type() == shared::MessageType::Connect)
			{
				registerClient(socket, msg);
				continue;
			}

        	handleMessage(msg);
		}
    }
}

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

void TcpServer::handleMessage(const shared::Message& msg) const
{
	if (msg.sender().isNull())
	{
		qWarning() << "Ignoring message with invalid UUID";
		return;
	}

	switch (msg.type())
	{
	case shared::MessageType::Text:
		{
			qInfo() << "Text message from" << msg.sender().toString() << ":" << msg.content();

			for (const auto& uuid : m_sockets.keys())
			{
				if (uuid == msg.sender()) continue;
				sendMessage(uuid, msg);
			}
		}
		break;
	case shared::MessageType::Command:
		{
			qInfo() << "Command from" << msg.sender().toString() << ":" << msg.content();
		}
		break;
	default:
		{
			qInfo() << "Unknown or unsupported message from" << msg.sender().toString() << ":" << msg.content();
		}
		break;
	}
}

void TcpServer::registerClient(QTcpSocket* socket, const shared::Message& msg)
{
	if (!socket) return;

	const QUuid uuid = msg.sender();

	if (m_sockets.contains(uuid))
	{
		qWarning() << "Client already registered";
		return;
	}

	m_sockets.insert(uuid, socket);

	qInfo() << "New client" << uuid.toString() << "connected";
	sendMessage(uuid, shared::Message(shared::MessageType::Text, m_uuid, "Hello, client!"));
}
