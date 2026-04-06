#pragma once

#include "Packet.h"
#include "AuthInfo.h"

namespace shared {

class PacketFactory
{
public:
    static Packet connectPacket(const QUuid& sender, const QUuid& target);
    static Packet textMessagePacket(const QUuid& sender, const QUuid& target, QString content);
    static Packet mediaMessagePacket(const QUuid& sender, const QUuid& target, QString content);
    static Packet registerPacket(const QUuid& sender, const QUuid& target, const RegisterInfo& info);
    static Packet loginPacket(const QUuid& sender, const QUuid& target, const LoginInfo& info);
    static Packet successPacket(const QUuid& sender, const QUuid& target, QString message);
    static Packet errorPacket(const QUuid& sender, const QUuid& target, QString message);
};

}
