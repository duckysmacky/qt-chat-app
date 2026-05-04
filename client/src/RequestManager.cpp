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
    case shared::PacketType::INVALID:
        emit invalidPacketReceived(packet);
        break;

    case shared::PacketType::CHAT_MESSAGE:
        {
            if (const auto& data = packet.data())
            {
                const auto message = shared::Message::deserialize(data.value());
                emit chatMessageReceived(message);
            }
        }
        break;

    case shared::PacketType::OPERATION_RESULT:
        {
            if (!packet.data().has_value()) break;

            const shared::OperationResult result = shared::OperationResult::deserialize(packet.data().value());
            const bool success = result.type() == shared::OperationResultType::SUCCESS;

            if (success)
                qInfo() << "Server result:" << result.text();
            else
                qWarning() << "Server result:" << result.text();

            emit operationResultReceived(result);
        }
        break;

    case shared::PacketType::USER_PROFILE_DATA:
        {
            if (!packet.data().has_value()) {
                qWarning() << "User profile data payload is missing";
                break;
            }

            const auto profile = shared::ProfileInfo::deserialize(packet.data().value());
            if (!profile.has_value()) {
                qWarning() << "Invalid user profile data payload";
                break;
            }

            emit currentUserProfileReceived(profile.value());
        }
        break;

    case shared::PacketType::PUBLIC_USER_INFO_DATA:
        {
            if (!packet.data().has_value()) {
                qWarning() << "Public user info data payload is missing";
                break;
            }

            const auto userInfo = shared::PublicUserInfo::deserialize(packet.data().value());
            if (!userInfo.has_value()) {
                qWarning() << "Invalid public user info data payload";
                break;
            }

            emit publicUserInfoReceived(userInfo.value());
        }
        break;

    case shared::PacketType::CHAT_LIST_DATA:
        {
            if (!packet.data().has_value()) {
                qWarning() << "Chat list data payload is missing";
                break;
            }

            const auto chats = shared::ChatsInfo::deserialize(packet.data().value());
            if (!chats.has_value()) {
                qWarning() << "Invalid chat list data payload";
                break;
            }

            emit chatListReceived(chats.value());
        }
        break;

    case shared::PacketType::CHAT_INFO_DATA:
        {
            if (!packet.data().has_value()) {
                qWarning() << "Chat info data payload is missing";
                break;
            }

            const auto chat = shared::ChatInfo::deserialize(packet.data().value());
            if (!chat.has_value()) {
                qWarning() << "Invalid chat info data payload";
                break;
            }

            emit chatInfoReceived(chat.value());
        }
        break;

    default:
        qWarning() << "Unknown or unsupported packet received";
        emit unsupportedPacketReceived(packet);
        break;
    }
}

void RequestManager::connectClient() const
{
    const Client& client = Client::instance();
    sendPacket(shared::PacketFactory::connectClientPacket(client.sessionId(), client.serverId()));
}

void RequestManager::sendServerCommand() const
{
    const Client& client = Client::instance();
    sendPacket(shared::Packet(shared::PacketType::SERVER_COMMAND, client.sessionId(), client.serverId()));
}

void RequestManager::sendServerCommand(QByteArray data) const
{
    const Client& client = Client::instance();
    sendPacket(shared::Packet(shared::PacketType::SERVER_COMMAND, client.sessionId(), client.serverId(), std::move(data)));
}

void RequestManager::sendChatMessage(shared::Message message) const
{
    const Client& client = Client::instance();
    sendPacket(shared::PacketFactory::chatMessagePacket(client.sessionId(), client.serverId(), std::move(message)));
}

void RequestManager::sendTextChatMessage(QString content) const
{
    sendChatMessage(shared::Message(shared::MessageType::TEXT, std::move(content)));
}

void RequestManager::sendMediaChatMessage(QString content) const
{
    sendChatMessage(shared::Message(shared::MessageType::MEDIA, std::move(content)));
}

void RequestManager::loginUser(QString login, QString passwordHash) const
{
    const Client& client = Client::instance();
    const shared::LoginInfo info(std::move(login), std::move(passwordHash));
    sendPacket(shared::PacketFactory::loginUserPacket(client.sessionId(), client.serverId(), info));
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
    sendPacket(shared::PacketFactory::registerUserPacket(client.sessionId(), client.serverId(), info));
}

void RequestManager::logoutCurrentUser() const
{
    const Client& client = Client::instance();
    sendPacket(shared::Packet(shared::PacketType::LOGOUT_USER, client.sessionId(), client.serverId()));
}

void RequestManager::getCurrentUserProfile() const
{
    const Client& client = Client::instance();
    sendPacket(shared::PacketFactory::getUserProfilePacket(client.sessionId(), client.serverId()));
}

void RequestManager::updateCurrentUserProfile(shared::ProfileUpdateInfo info) const
{
    const Client& client = Client::instance();
    sendPacket(shared::PacketFactory::updateUserProfilePacket(client.sessionId(), client.serverId(), std::move(info)));
}

void RequestManager::getPublicUserInfo(const QUuid& userId) const
{
    const Client& client = Client::instance();
    sendPacket(shared::PacketFactory::getPublicUserInfoPacket(client.sessionId(), client.serverId(), userId));
}

void RequestManager::getCurrentUserChats() const
{
    const Client& client = Client::instance();
    sendPacket(shared::PacketFactory::getChatsPacket(client.sessionId(), client.serverId()));
}

void RequestManager::searchChats(QString query) const
{
    const Client& client = Client::instance();
    sendPacket(shared::PacketFactory::searchChatsPacket(client.sessionId(), client.serverId(), std::move(query)));
}

void RequestManager::createChat(shared::ChatCreateInfo info) const
{
    const Client& client = Client::instance();
    sendPacket(shared::PacketFactory::createChatPacket(client.sessionId(), client.serverId(), std::move(info)));
}

void RequestManager::createChat(QString type, QString title) const
{
    createChat(shared::ChatCreateInfo(std::move(type), std::move(title)));
}

void RequestManager::sendPacket(shared::Packet packet) const
{
    Client::instance().sendBytes(shared::util::encapsulate(packet));
}
