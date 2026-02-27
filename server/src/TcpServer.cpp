#include "TcpServer.h"

#include <QByteArray>
#include <QString>
#include <QDebug>

TcpServer::TcpServer(QObject* parent) 
	: QObject(parent),
	  m_server(new QTcpServer(this)),
	  m_socket(nullptr),
	  m_isRunning(false)
{
	connect(m_server, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);
}

bool TcpServer::start(const uint16_t port)
{
	if (m_server->listen(QHostAddress::Any, port))
	{
		qInfo() << "Server started and listening on" << port;
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


void TcpServer::onNewConnection()
{
	m_socket = m_server->nextPendingConnection();

	connect(m_socket, &QTcpSocket::readyRead, this, &TcpServer::onServerRead);
	connect(m_socket, &QTcpSocket::disconnected, this, &TcpServer::onClientDisconnected);

	m_socket->write("Hello, client!\r\n");
}

void TcpServer::onServerRead() const
{
    QString message = "";

    while(m_socket->bytesAvailable() > 0)
    {
        QByteArray bytes = m_socket->readAll();
        qDebug() << "Incoming bytes:" << bytes << "\n";

        if (bytes == "\x01")
        {
            m_socket->write(message.toUtf8());
            message = "";
        }
        else
            message.append(bytes);
    }

    m_socket->write(message.toUtf8());
}

void TcpServer::onClientDisconnected()
{
    m_socket->close();
	m_socket = nullptr;
}