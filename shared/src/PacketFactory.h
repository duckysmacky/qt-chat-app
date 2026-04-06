#pragma once

#include "Packet.h"
#include "AuthInfo.h"

namespace shared {

class PacketFactory
{
public:
    static Packet connectPacket(QUuid sender, QUuid target);
    static Packet textMessagePacket(QUuid sender, QUuid target, QString content);
    static Packet mediaMessagePacket(QUuid sender, QUuid target, QString content);
    static Packet registerPacket(QUuid sender, QUuid target, const RegisterInfo& info);
    static Packet loginPacket(QUuid sender, QUuid target, const LoginInfo& info);
    static Packet successPacket(QUuid sender, QUuid target, QString message);
    static Packet errorPacket(QUuid sender, QUuid target, QString message);
};

}
