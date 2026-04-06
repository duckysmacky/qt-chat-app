#include "Client.h"

#include <QAbstractSocket>

#include "Packet.h"
#include "util.h"

/**
 * @brief Returns the singleton client instance.
 * @return Reference to the Client instance.
 */
Client& Client::instance()
{
    static Client instance;
    return instance;
}

/**
 * @brief Constructs the Client.
 * @param parent The parent QObject.
 */
Client::Client(QObject* parent)
    : QObject(parent),
      m_uuid(QUuid::createUuid()),
      m_socket(this),
      m_connected(false)
{
    connect(&m_socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(&m_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    connect(&m_socket, &QTcpSocket::errorOccurred, this, &Client::onErrorOccurred);
    connect(&m_socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
}

/**
 * @brief Connects the client to a server.
 * @param host The server's host address.
 * @param port The server's port number.
 */
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

/**
 * @brief Disconnects the client from the server.
 */
void Client::disconnect()
{
    setStatusText("Disconnecting...");
    m_socket.disconnectFromHost();
}

/**
 * @brief Sends a message to the server.
 * @param type The type of the message.
 * @param content The content of the message.
 */
void Client::sendMessage(QString content)
{
    const shared::Message message(shared::MessageType::TEXT, std::move(content));
    sendPacket(shared::PacketType::MESSAGE, message.serialize());
}

/**
 * @brief Handles the connected signal from the socket.
 */
void Client::onConnected()
{
    sendPacket(shared::PacketType::CONNECT);

    setStatusText("Connected");
    setConnectionStatus(true);
}

/**
 * @brief Handles the disconnected signal from the socket.
 */
void Client::onDisconnected()
{
    setStatusText("Disconnected");
    setConnectionStatus(false);
}

/**
 * @brief Handles socket errors.
 * @param error The socket error type.
 */
void Client::onErrorOccurred(QAbstractSocket::SocketError)
{
    setStatusText(m_socket.errorString());
    if (m_socket.state() != QAbstractSocket::ConnectedState)
        setConnectionStatus(false);
}

/**
 * @brief Handles incoming data from the socket.
 */
void Client::onReadyRead()
{
    const QByteArray bytes = m_socket.readAll();
    if (bytes.isEmpty()) return;

    const QList<shared::Packet> packets = shared::util::parse(bytes);
    for (const auto& packet : packets)
    {
        switch (packet.type())
        {
        case shared::PacketType::MESSAGE:
            {
                if (auto data = packet.data())
                {
                    const auto msg = shared::Message::deserialize(data.value());
                    emit messageReceived(packet.sender(), msg);
                }
            }
            break;
        default:
            {
                qWarning() << "Unknown or unsupported packet received";
            }
            break;
        }
    }
}

void Client::sendPacket(const shared::PacketType type)
{
    qInfo() << "Sending packet to server";
    const shared::Packet packet(type, m_uuid, QUuid());

    QByteArray payload;
    payload.append(packet.serialize());
    payload.append(shared::util::DELIMITER);

    m_socket.write(payload);
}

void Client::sendPacket(const shared::PacketType type, QByteArray data)
{
    qInfo() << "Sending packet to server";
    const shared::Packet packet(type, m_uuid, QUuid(), std::move(data));

    QByteArray payload;
    payload.append(packet.serialize());
    payload.append(shared::util::DELIMITER);

    m_socket.write(payload);
}

/**
 * @brief Sets the status text.
 * @param text The new status text.
 */
void Client::setStatusText(const QString& text)
{
    if (m_statusText == text) return;

    m_statusText = text;
    emit statusTextChanged();
}

/**
 * @brief Sets the connection status.
 * @param connected The new connection status.
 */
void Client::setConnectionStatus(bool connected)
{
    m_connected = connected;
    emit connectionStatusChanged();
}
