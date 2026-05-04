#include "RequestManager.h"

#include <QDebug>
#include <QUuid>

#include <utility>

#include "Client.h"
#include "PacketFactory.h"
#include "dto/AuthInfo.h"
#include "util.h"

RequestManager& RequestManager::instance()
{
    static RequestManager instance;
    return instance;
}

RequestManager::RequestManager(QObject* parent)
    : QObject(parent)
{}

void RequestManager::processBytes(const QByteArray& bytes)
{
    const QList<shared::Packet> packets = shared::util::parse(bytes);
    for (const auto& packet : packets)
        processPacket(packet);
}

void RequestManager::processPacket(const shared::Packet& packet)
{
    switch (packet.type())
    {
    case shared::PacketType::MESSAGE:
        {
            if (const auto& data = packet.data())
            {
                const auto message = shared::Message::deserialize(data.value());
                emit messageReceived(message);
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

            emit resultReceived(result);
        }
        break;
    default:
        {
            qWarning() << "Unknown or unsupported packet received";
        }
        break;
    }
}

void RequestManager::sendConnect() const
{
    const Client& client = Client::instance();
    sendPacket(shared::PacketFactory::connectPacket(client.sessionId(), client.serverId()));
}

void RequestManager::sendMessage(shared::Message message) const
{
    const Client& client = Client::instance();
    sendPacket(shared::PacketFactory::messagePacket(client.sessionId(), client.serverId(), std::move(message)));
}

void RequestManager::sendTextMessage(QString content) const
{
    sendMessage(shared::Message(shared::MessageType::TEXT, std::move(content)));
}

void RequestManager::login(QString login, QString passwordHash) const
{
    const Client& client = Client::instance();
    const shared::LoginInfo info(std::move(login), std::move(passwordHash));
    sendPacket(shared::PacketFactory::loginPacket(client.sessionId(), client.serverId(), info));
}

void RequestManager::registerUser(QString username, QString name, QString email, QString passwordHash) const
{
    const Client& client = Client::instance();
    const shared::RegisterInfo info(
        std::move(username),
        std::move(name),
        std::move(email),
        std::move(passwordHash)
    );
    sendPacket(shared::PacketFactory::registerPacket(client.sessionId(), client.serverId(), info));
}

void RequestManager::logout() const
{
    const Client& client = Client::instance();
    sendPacket(shared::Packet(shared::PacketType::LOGOUT, client.sessionId(), client.serverId()));
}

void RequestManager::sendPacket(shared::Packet packet) const
{
    Client::instance().sendBytes(shared::util::encapsulate(packet));
}
