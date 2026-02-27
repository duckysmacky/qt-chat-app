#include "TcpServer.h"

#include <QByteArray>
#include <QDebug>

TcpServer::TcpServer(QObject* parent) 
	: QObject(parent),
	  m_server(new QTcpServer(this)),
	  m_consoleReader(new ConsoleReader(this)),
	  m_isRunning(false)
{
	connect(m_server, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);
	connect(m_consoleReader, &ConsoleReader::lineRead, this, [this](const QString& line) {
		if (!line.isEmpty())
			broadcast(line + "\r\n");
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

void TcpServer::broadcast(const QString& message) const
{
	for (const auto socket : m_sockets)
	{
		qDebug() << "Sending" << message << "to [" << socket->socketDescriptor() << "]";

		if (socket->write(message.toUtf8()) == -1)
		{
			qCritical() << "Error writing to socket [" << socket->socketDescriptor() << "]:" << socket->errorString();
		}
	}
}

void TcpServer::onNewConnection()
{
	QTcpSocket* socket = m_server->nextPendingConnection();
	if (!socket) return;

	const qintptr descriptor = socket->socketDescriptor();
	m_sockets.insert(descriptor, socket);

	connect(socket, &QTcpSocket::readyRead, this, &TcpServer::onServerRead);
	connect(socket, &QTcpSocket::disconnected, this, &TcpServer::onClientDisconnected);

	qInfo() << "New client [" << descriptor << "] connected";

	socket->write("Hello, client!\r\n");
}

void TcpServer::onServerRead() const
{
	auto* socket = qobject_cast<QTcpSocket*>(sender());
	if (!socket) return;

    QString message = "";

    while (socket->bytesAvailable() > 0)
    {
        QByteArray bytes = socket->readAll();
        qDebug() << "Incoming bytes:" << bytes << "\n";

        if (bytes == "\x01")
        {
            socket->write(message.toUtf8());
            message = "";
        }
        else
            message.append(bytes);
    }

	qInfo() << "Incoming message from [" << socket->socketDescriptor() << "]:" << message;

    socket->write(message.toUtf8());
}

void TcpServer::onClientDisconnected()
{
	auto* socket = qobject_cast<QTcpSocket*>(sender());
	if (!socket) return;

	const qintptr descriptor = socket->socketDescriptor();
	m_sockets.remove(descriptor);

	qInfo() << "Client [" << descriptor << "] disconnected";

	socket->close();
	socket->deleteLater();
}
