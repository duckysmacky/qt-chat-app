#include "TcpServer.h"

#include <QByteArray>
#include <QDebug>

TcpServer::TcpServer(QObject* parent) 
	: QObject(parent),
	  m_server(new QTcpServer(this)),
	  m_isRunning(false)
{
	connect(m_server, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);
}

bool TcpServer::start(const uint16_t port)
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

void TcpServer::stop() const
{
	if (m_isRunning)
		m_server->close();
}

void TcpServer::broadcast(const QString& message) const
{
	for (const auto socket : m_sockets)
	{
		if (socket->write(message.toUtf8()) == -1)
		{
			qCritical() << "Error writing to socket" << socket->localAddress() << ":" << socket->errorString();
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

    socket->write(message.toUtf8());
}

void TcpServer::onClientDisconnected()
{
	auto* socket = qobject_cast<QTcpSocket*>(sender());
	if (!socket) return;

	const qintptr descriptor = socket->socketDescriptor();
	m_sockets.remove(descriptor);

	socket->close();
	socket->deleteLater();
}
