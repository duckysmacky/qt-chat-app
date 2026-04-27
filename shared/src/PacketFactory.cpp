#include "PacketFactory.h"

#include "Message.h"
#include "Result.h"
#include "dto/AuthInfo.h"
#include "dto/ProfileInfo.h"
#include "dto/ProfileUpdateInfo.h"
#include "dto/PublicUserInfo.h"



namespace shared {

Packet PacketFactory::connectPacket(const QUuid& sender, const QUuid& receiver)
{
    return {PacketType::CONNECT, sender, receiver};
}


Packet PacketFactory::messagePacket(const QUuid& sender, const QUuid& receiver, Message message)
{
    return Packet{PacketType::MESSAGE, sender, receiver, message.serialize()};
}

Packet PacketFactory::textMessagePacket(const QUuid& sender, const QUuid& receiver, QString content)
{
    const Message message(MessageType::TEXT, std::move(content));
    return messagePacket(sender, receiver, message);
}

Packet PacketFactory::mediaMessagePacket(const QUuid& sender, const QUuid& receiver, QString content)
{
    const Message message(MessageType::MEDIA, std::move(content));
    return messagePacket(sender, receiver, message);
}

Packet PacketFactory::registerPacket(const QUuid& sender, const QUuid& receiver, RegisterInfo info)
{
    return Packet{PacketType::REGISTER, sender, receiver, info.serialize()};
}
Packet PacketFactory::loginPacket(const QUuid& sender, const QUuid& receiver, LoginInfo info)
{
    return Packet{PacketType::LOGIN, sender, receiver, info.serialize()};
}

Packet PacketFactory::profileRequestPacket(const QUuid& sender, const QUuid& receiver)
{
    return Packet{PacketType::PROFILE_REQUEST, sender, receiver};
}

Packet PacketFactory::profileUpdatePacket(const QUuid& sender, const QUuid& receiver, ProfileUpdateInfo info)
{
    return Packet{PacketType::PROFILE_UPDATE, sender, receiver, info.serialize()};
}

Packet PacketFactory::profileDataPacket(const QUuid& sender, const QUuid& receiver, ProfileInfo info)
{
    return Packet{PacketType::PROFILE_DATA, sender, receiver, info.serialize()};
}

Packet PacketFactory::userInfoRequestPacket(const QUuid& sender, const QUuid& receiver, const QUuid& userId)
{
    return Packet{
        PacketType::USER_INFO_REQUEST,
        sender,
        receiver,
        userId.toRfc4122()
    };
}

Packet PacketFactory::userInfoDataPacket(const QUuid& sender, const QUuid& receiver, PublicUserInfo info)
{
    return Packet{PacketType::USER_INFO_DATA, sender, receiver, info.serialize()};
}

Packet PacketFactory::successPacket(const QUuid& sender, const QUuid& receiver, QString message)
{
    const Result result(ResultType::SUCCESS, std::move(message));
    return Packet{PacketType::RESULT, sender, receiver, result.serialize()};
}

Packet PacketFactory::errorPacket(const QUuid& sender, const QUuid& receiver, QString message)
{
    const Result result(ResultType::ERROR, std::move(message));
    return Packet{PacketType::RESULT, sender, receiver, result.serialize()};
}

}
