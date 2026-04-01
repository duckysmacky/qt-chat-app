#pragma once

#include <Packet.h>

namespace shared {

class PacketFactory
{
public:
    static Packet connectPacket(QUuid sender, QUuid target);
    static Packet textMessagePacket(QUuid sender, QUuid target, QString content);
    static Packet mediaMessagePacket(QUuid sender, QUuid target, QString content);
};

}