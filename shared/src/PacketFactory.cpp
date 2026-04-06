#include "PacketFactory.h"

#include "Message.h"
#include "Result.h"
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
    const Result result(ResultType::SUCCESS, std::move(message));
    return Packet(PacketType::RESULT, std::move(sender), std::move(target), result.serialize());
}

Packet PacketFactory::errorPacket(QUuid sender, QUuid target, QString message)
{
    const Result result(ResultType::ERROR, std::move(message));
    return Packet(PacketType::RESULT, std::move(sender), std::move(target), result.serialize());
}

}
