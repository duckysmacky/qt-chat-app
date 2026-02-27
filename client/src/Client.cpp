#include "Client.h"

#include <QAbstractSocket>

Client::Client(QObject* parent)
    : QObject(parent),
      m_socket(this)
{
    connect(&m_socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(&m_socket, &QTcpSocket::errorOccurred, this, &Client::onErrorOccurred);
}

void Client::connectTo(const QString& host, const int port)
{
    if (host.isEmpty() || port <= 0 || port > 65535)
    {
        setStatusText("Invalid host or port");
        return;
    }

    if (m_socket.state() != QAbstractSocket::UnconnectedState)
        m_socket.abort();

    setStatusText("Connecting...");
    m_socket.connectToHost(host, static_cast<quint16>(port));
}

void Client::onConnected()
{
    setStatusText("Connected");
}

void Client::onErrorOccurred(QAbstractSocket::SocketError)
{
    setStatusText(m_socket.errorString());
}

void Client::setStatusText(const QString& text)
{
    if (m_statusText == text)
        return;

    m_statusText = text;
    emit statusTextChanged();
}
