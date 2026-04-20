#include "Client.h"

#include <QAbstractSocket>

#include "AuthInfo.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Result.h"
#include "util.h"

Client& Client::instance()
{
    static Client instance;
    return instance;
}

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

void Client::sendMessage(QString content)
{
    const shared::Message message(shared::MessageType::TEXT, std::move(content));
    sendPacket(shared::PacketType::MESSAGE, message.serialize());
}

void Client::login(QString login, QString passwordHash)
{
    const shared::LoginInfo info(std::move(login), std::move(passwordHash));
    sendPacket(shared::PacketType::LOGIN, info.serialize());
}

void Client::registerUser(QString username, QString name, QString email, QString passwordHash)
{
    const shared::RegisterInfo info(
        std::move(username),
        std::move(name),
        std::move(email),
        std::move(passwordHash)
    );
    sendPacket(shared::PacketType::REGISTER, info.serialize());
}

void Client::onConnected()
{
    sendPacket(shared::PacketType::CONNECT);

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
                    emit messageReceived(packet.sender().toString(), msg);
                }
            }
            break;
        case shared::PacketType::RESULT:
            {
                if (!packet.data().has_value()) break;

                const shared::Result result = shared::Result::deserialize(packet.data().value());
                const bool success = result.type() == shared::ResultType::SUCCESS;

                if (success)
                    qInfo() << "Server result:" << result.text();
                else
                    qWarning() << "Server result:" << result.text();

                emit resultReceived(success, result.text());
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
    m_socket.write(shared::util::encapsulate(packet));
}

void Client::sendPacket(const shared::PacketType type, QByteArray data)
{
    qInfo() << "Sending packet to server";

    const shared::Packet packet(type, m_uuid, QUuid(), std::move(data));
    m_socket.write(shared::util::encapsulate(packet));
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
