#include "PacketFactory.h"

#include "Message.h"
#include "AuthInfo.h"
namespace shared {

Packet PacketFactory::connectPacket(QUuid sender, QUuid target)
{
    return Packet(PacketType::CONNECT, std::move(sender), std::move(target));
}

Packet PacketFactory::textMessagePacket(QUuid sender, QUuid target, QString content)
{
    const Message message(MessageType::TEXT, std::move(content));
    return Packet(PacketType::MESSAGE, std::move(sender), std::move(target), std::move(message.serialize()));
}

Packet PacketFactory::mediaMessagePacket(QUuid sender, QUuid target, QString content)
{
    const Message message(MessageType::MEDIA, std::move(content));
    return Packet(PacketType::MESSAGE, std::move(sender), std::move(target), std::move(message.serialize()));
}
Packet PacketFactory::registerPacket(QUuid sender, QUuid target, const RegisterInfo& info)
{
    return Packet(PacketType::REGISTER, std::move(sender), std::move(target), info.serialize());
}

Packet PacketFactory::loginPacket(QUuid sender, QUuid target, const LoginInfo& info)
{
    return Packet(PacketType::LOGIN, std::move(sender), std::move(target), info.serialize());
}

Packet PacketFactory::successPacket(QUuid sender, QUuid target, QString message)
{
    return Packet(PacketType::SUCCESS, std::move(sender), std::move(target), message.toUtf8());
}

Packet PacketFactory::errorPacket(QUuid sender, QUuid target, QString message)
{
    return Packet(PacketType::ERROR, std::move(sender), std::move(target), message.toUtf8());
}

}
