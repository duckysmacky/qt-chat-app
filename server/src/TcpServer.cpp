#include <QByteArray>
#include <QDebug>

#include "TcpServer.h"
#include "middleware.h"
#include "Message.h"

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

void TcpServer::broadcast(const QString& text) const
{
	const shared::Message msg(shared::MessageType::Text, text);
	QByteArray bytes;
	bytes.append(msg.encode());
	bytes.append('\x01');

	for (const auto socket : m_sockets)
	{
		qDebug() << "Sending" << text << "to [" << socket->socketDescriptor() << "]";

		if (socket->write(bytes) == -1)
			qCritical() << "Error writing to socket [" << socket->socketDescriptor() << "]:" << socket->errorString();
	}
}

void TcpServer::onNewConnection()
{
	QTcpSocket* socket = m_server->nextPendingConnection();
	if (!socket) return;

	const qintptr descriptor = socket->socketDescriptor();
	m_sockets.insert(descriptor, socket);
    m_buffers.insert(descriptor, "");

	connect(socket, &QTcpSocket::readyRead, this, &TcpServer::onServerRead);
	connect(socket, &QTcpSocket::disconnected, this, &TcpServer::onClientDisconnected);

	qInfo() << "New client [" << descriptor << "] connected";

	const shared::Message msg(shared::MessageType::Text, "Hello, client!");
	socket->write(msg.encode());
}

void TcpServer::onServerRead() const
{
	auto* socket = qobject_cast<QTcpSocket*>(sender());
	if (!socket) return;

    const auto descriptor = socket->socketDescriptor();

    while (socket->bytesAvailable() > 0)
    {
        const QByteArray bytes = socket->readAll();
        const QList<shared::Message> messages = middleware::parse(bytes);

        for (const auto& msg : messages)
        {
            if (msg.type() == shared::MessageType::Text)
            {
                qInfo() << "Text message from [" << descriptor << "]:" << msg.content();
            }
            else if (msg.type() == shared::MessageType::Command)
            {
                qInfo() << "Command from [" << descriptor << "]:" << msg.content();
            }
        }
    }
}

void TcpServer::onClientDisconnected()
{
	auto* socket = qobject_cast<QTcpSocket*>(sender());
	if (!socket) return;

	const qintptr descriptor = socket->socketDescriptor();
	m_sockets.remove(descriptor);
    m_buffers.remove(descriptor);

	qInfo() << "Client [" << descriptor << "] disconnected";

	socket->close();
	socket->deleteLater();
}
