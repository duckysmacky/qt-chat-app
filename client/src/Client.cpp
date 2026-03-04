#include "Client.h"

#include <QAbstractSocket>
#include <QRegularExpression>

Client::Client(QObject* parent)
    : QObject(parent),
      m_socket(this)
{
    connect(&m_socket, &QTcpSocket::connected, this, &Client::onConnected);
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

void Client::sendMessage(const QString& message)
{
    if (message.trimmed().isEmpty())
        return;

    qInfo() << "Message sent:" << message;

    m_socket.write(message.toUtf8());

    emit messageReceived(message);
}

void Client::onConnected()
{
    setStatusText("Connected");
}

void Client::onErrorOccurred(QAbstractSocket::SocketError)
{
    setStatusText(m_socket.errorString());
}

void Client::onReadyRead()
{
    const QByteArray bytes = m_socket.readAll();
    if (bytes.isEmpty()) return;

    const QString message = QString::fromUtf8(bytes);
    const QStringList parts = message.split(QRegularExpression("[\\r\\n]+"), Qt::SkipEmptyParts);

    if (parts.isEmpty()) {
        appendMessage(message);
    } else {
        for (const QString& part : parts) appendMessage(part);
    }
}

void Client::setStatusText(const QString& text)
{
    if (m_statusText == text) return;

    m_statusText = text;
    emit statusTextChanged();
}

void Client::appendMessage(const QString& message)
{
    if (message.isEmpty()) return;

    m_messages.append(message);
    emit messagesChanged();
    emit messageReceived(message);
}
