#include "TcpServer.h"

#include <QTcpSocket>
#include <QtNetwork>
#include <QByteArray>
#include <QString>
#include <QDebug>
#include <cstdint>

TcpServer::TcpServer(QObject* parent) 
	: QObject(parent),
	  m_server(new QTcpServer(this)),
		m_running(false)
{
	connect(m_server, &QTcpServer::newConnection, this, &TcpServer::on_new_connection);
}

TcpServer::~TcpServer()
{
	m_server->close();
	delete m_server;
	//server_status=0;
}

void TcpServer::start(uint16_t port)
{
	if(m_server->listen(QHostAddress::Any, port))
	{
		qDebug() << "Server started and listening on" << port;
		m_running = true;
	} 
	else
		qDebug() << "Server not started";
}


void TcpServer::on_new_connection()
{
	m_socket = m_server->nextPendingConnection();

	connect(m_socket, &QTcpSocket::readyRead, this, &TcpServer::on_server_read);
	connect(m_socket, &QTcpSocket::disconnected, this, &TcpServer::on_client_disconnected);

	m_socket->write("Hello, client!\r\n");
}

void TcpServer::on_server_read()
{
    QString res = "";

    while(m_socket->bytesAvailable() > 0)
    {
        QByteArray array = m_socket->readAll();
        qDebug() << array << "\n";

        if(array == "\x01")
        {
            m_socket->write(res.toUtf8());
            res = "";
        }
        else
            res.append(array);
    }

    m_socket->write(res.toUtf8());
}

void TcpServer::on_client_disconnected()
{
    m_socket->close();
}

