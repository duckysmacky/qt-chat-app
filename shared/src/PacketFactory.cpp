#include "PacketFactory.h"

#include "Message.h"

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
}
