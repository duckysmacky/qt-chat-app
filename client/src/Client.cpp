#include "Client.h"

#include <QAbstractSocket>
#include <QDebug>
#include <QMetaObject>

#include <utility>

#include "RequestManager.h"

Client& Client::instance()
{
    static Client instance;
    return instance;
}

Client::Client(QObject* parent)
    : QObject(parent),
      m_sessionId(QUuid::createUuid()),
      m_serverId(QUuid{}),
      m_socket(this),
      m_connected(false)
{
    connect(&m_socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(&m_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    connect(&m_socket, &QTcpSocket::errorOccurred, this, &Client::onErrorOccurred);
    connect(&m_socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
}

void Client::connectTo(const QString& host, const int port)
{
    if (host.isEmpty() || port <= 0 || port > 65535)
    {
        setStatusText("Invalid host or port");
        return;
    }

    if (m_socket.state() != QAbstractSocket::UnconnectedState)
        disconnect();

    setStatusText("Connecting...");
    m_socket.connectToHost(host, static_cast<quint16>(port));
}

void Client::disconnect()
{
    setStatusText("Disconnecting...");
    m_socket.disconnectFromHost();
}

QString Client::resolveUserData(const QUuid &userId) const
{
    // TODO
    return userId.toString();
}

void Client::onConnected()
{
    RequestManager::instance().sendConnect();

    setStatusText("Connected");
    setConnectionStatus(true);
}

void Client::onDisconnected()
{
    setStatusText("Disconnected");
    setConnectionStatus(false);
}

void Client::onErrorOccurred(QAbstractSocket::SocketError)
{
    setStatusText(m_socket.errorString());
    if (m_socket.state() != QAbstractSocket::ConnectedState)
        setConnectionStatus(false);
}

void Client::onReadyRead()
{
    const QByteArray bytes = m_socket.readAll();
    if (bytes.isEmpty()) return;

    RequestManager::instance().processBytes(bytes);
}

void Client::sendBytes(QByteArray bytes)
{
    QMetaObject::invokeMethod(
        this,
        [this, bytes = std::move(bytes)] {
            writeBytes(bytes);
        },
        Qt::QueuedConnection
    );
}

void Client::writeBytes(const QByteArray& bytes)
{
    m_socket.write(bytes);
}

void Client::setStatusText(const QString& text)
{
    if (m_statusText == text) return;

    m_statusText = text;
    emit statusTextChanged();
}

void Client::setConnectionStatus(bool connected)
{
    m_connected = connected;
    emit connectionStatusChanged();
}
