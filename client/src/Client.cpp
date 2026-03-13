#include "Client.h"

#include <QAbstractSocket>
#include <QRegularExpression>

#include "Message.h"
#include "utilus.h"

Client::Client(QObject* parent)
    : QObject(parent),
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
				m_socket.abort();

		setStatusText("Connecting...");
    m_socket.connectToHost(host, static_cast<quint16>(port));
}

void Client::disconnect()
{
		if (m_socket.state() != QAbstractSocket::ConnectedState)
				m_socket.abort();

		setStatusText("Disconnecting...");
		m_socket.disconnectFromHost();
}

void Client::sendMessage(const QString& text)
{
    if (text.trimmed().isEmpty())
        return;

    qInfo() << "Message sent:" << text;

    const shared::Message message(shared::MessageType::Text, text);
    QByteArray bytes;
    bytes.append(message.encode());
    bytes.append('\x01');

    m_socket.write(bytes);

}

void Client::onConnected()
{
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
    // TODO: add support for multiple messages, like on the server side (X)
    // вроде бы я сделал то что нужно, если что не бейте (((
    const QByteArray bytes = m_socket.readAll();
    if (bytes.isEmpty()) return;

    const QList<shared::Message> messages = shared::parse(bytes);

    for (const auto& msg : messages){

        if (msg.type() == shared::MessageType::Text)
        {
            const QStringList parts = msg.content().split(QRegularExpression("[\\r\\n]+"), Qt::SkipEmptyParts);

            if (parts.isEmpty()) {
                appendMessage(msg.content());
            } else {
                for (const QString& part : parts) appendMessage(part);
            }
        }
        else
        {
            appendMessage("[Unknown message received]");
        }

    }


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

void Client::appendMessage(const QString& message)
{
    if (message.isEmpty()) return;

    m_messages.append(message);
    emit messagesChanged();
}
