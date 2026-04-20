#include "PacketFactory.h"

#include "Message.h"
#include "Result.h"
#include "AuthInfo.h"

namespace shared {

/**
 * @brief Creates a CONNECT packet.
 */
Packet PacketFactory::connectPacket(const QUuid& sender, const QUuid& target)
{
    return {PacketType::CONNECT, sender, target};
}

/**
 * @brief Creates a text message packet.
 */
Packet PacketFactory::textMessagePacket(const QUuid& sender, const QUuid& target, QString content)
{
    const Message message(MessageType::TEXT, std::move(content));
    return Packet{PacketType::MESSAGE, sender, target, message.serialize()};
}

/**
 * @brief Creates a media message packet.
 */
Packet PacketFactory::mediaMessagePacket(const QUuid& sender, const QUuid& target, QString content)
{
    const Message message(MessageType::MEDIA, std::move(content));
    return Packet{PacketType::MESSAGE, sender, target, message.serialize()};
}

/**
 * @brief Creates a register request packet.
 */
Packet PacketFactory::registerPacket(const QUuid& sender, const QUuid& target, const RegisterInfo& info)
{
    return Packet{PacketType::REGISTER, sender, target, info.serialize()};
}

/**
 * @brief Creates a login request packet.
 */
Packet PacketFactory::loginPacket(const QUuid& sender, const QUuid& target, const LoginInfo& info)
{
    return Packet{PacketType::LOGIN, sender, target, info.serialize()};
}

/**
 * @brief Creates a success result packet.
 */
Packet PacketFactory::successPacket(const QUuid& sender, const QUuid& target, QString message)
{
    const Result result(ResultType::SUCCESS, std::move(message));
    return Packet{PacketType::RESULT, sender, target, result.serialize()};
}

/**
 * @brief Creates an error result packet.
 */
Packet PacketFactory::errorPacket(const QUuid& sender, const QUuid& target, QString message)
{
    const Result result(ResultType::ERROR, std::move(message));
    return Packet{PacketType::RESULT, sender, target, result.serialize()};
}

}