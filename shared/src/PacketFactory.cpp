#include "PacketFactory.h"

#include "Message.h"
#include "Result.h"
#include "AuthInfo.h"
#include "ProfileInfo.h"
#include "ProfileUpdateInfo.h"
#include "PublicUserInfo.h"

namespace shared {

Packet PacketFactory::connectPacket(const QUuid& sender, const QUuid& target)
{
    return {PacketType::CONNECT, sender, target};
}

Packet PacketFactory::messagePacket(const QUuid& sender, const QUuid& target, const Message& message)
{
    return Packet{PacketType::MESSAGE, sender, target, message.serialize()};
}

Packet PacketFactory::textMessagePacket(const QUuid& sender, const QUuid& target, QString content)
{
    const Message message(MessageType::TEXT, std::move(content));
    return messagePacket(sender, target, message);
}

Packet PacketFactory::mediaMessagePacket(const QUuid& sender, const QUuid& target, QString content)
{
    const Message message(MessageType::MEDIA, std::move(content));
    return messagePacket(sender, target, message);
}

Packet PacketFactory::registerPacket(const QUuid& sender, const QUuid& target, const RegisterInfo& info)
{
    return Packet{PacketType::REGISTER, sender, target, info.serialize()};
}

Packet PacketFactory::loginPacket(const QUuid& sender, const QUuid& target, const LoginInfo& info)
{
    return Packet{PacketType::LOGIN, sender, target, info.serialize()};
}

Packet PacketFactory::profileRequestPacket(const QUuid& sender, const QUuid& target)
{
    return Packet{PacketType::PROFILE_REQUEST, sender, target};
}

Packet PacketFactory::profileUpdatePacket(const QUuid& sender, const QUuid& target, const ProfileUpdateInfo& info)
{
    return Packet{PacketType::PROFILE_UPDATE, sender, target, info.serialize()};
}

Packet PacketFactory::profileDataPacket(const QUuid& sender, const QUuid& target, const ProfileInfo& info)
{
    return Packet{PacketType::PROFILE_DATA, sender, target, info.serialize()};
}

Packet PacketFactory::userInfoRequestPacket(const QUuid& sender, const QUuid& target, const QUuid& userId)
{
    return Packet{
        PacketType::USER_INFO_REQUEST,
        sender,
        target,
        userId.toString(QUuid::WithoutBraces).toUtf8()
    };
}

Packet PacketFactory::userInfoDataPacket(const QUuid& sender, const QUuid& target, const PublicUserInfo& info)
{
    return Packet{PacketType::USER_INFO_DATA, sender, target, info.serialize()};
}

Packet PacketFactory::successPacket(const QUuid& sender, const QUuid& target, QString message)
{
    const Result result(ResultType::SUCCESS, std::move(message));
    return Packet{PacketType::RESULT, sender, target, result.serialize()};
}

Packet PacketFactory::errorPacket(const QUuid& sender, const QUuid& target, QString message)
{
    const Result result(ResultType::ERROR, std::move(message));
    return Packet{PacketType::RESULT, sender, target, result.serialize()};
}

}
