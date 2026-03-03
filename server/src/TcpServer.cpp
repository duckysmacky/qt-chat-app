#include "TcpServer.h"
#include "Middleware.h"
#include "Message.h"
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
    m_buffers.insert(descriptor, "");

	connect(socket, &QTcpSocket::readyRead, this, &TcpServer::onServerRead);
	connect(socket, &QTcpSocket::disconnected, this, &TcpServer::onClientDisconnected);

	qInfo() << "New client [" << descriptor << "] connected";

	socket->write("Hello, client!\r\n");
}

void TcpServer::onServerRead()
{
	auto* socket = qobject_cast<QTcpSocket*>(sender());
	if (!socket) return;

    const auto descriptor = socket->socketDescriptor();

    while(socket->bytesAvailable() > 0){
        const QByteArray bytes = socket->readAll();
        qDebug() << "vsem privet pashalka";
        const QList<QByteArray> messages = middleware::parse(bytes);

        for(const auto& message : messages){
            shared::Message mes = shared::Message::decode(message);
            if (mes.type() == shared::MessageType::Text) {
                qInfo() << "Text message" << mes.content();
            }
            else if (mes.type() == shared::MessageType::Command) {
                qInfo() << "Command message" << mes.content();
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
