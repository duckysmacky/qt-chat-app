#pragma once

#include "Packet.h"
#include "Message.h"
#include "dto/AuthInfo.h"
#include "dto/ProfileInfo.h"
#include "dto/ProfileUpdateInfo.h"
#include "dto/PublicUserInfo.h"


namespace shared {

class PacketFactory
{
public:
    static Packet connectPacket(const QUuid& sender, const QUuid& receiver);
    static Packet messagePacket(const QUuid& sender, const QUuid& receiver, Message message);
    static Packet textMessagePacket(const QUuid& sender, const QUuid& receiver, QString content);
    static Packet mediaMessagePacket(const QUuid& sender, const QUuid& receiver, QString content);
    static Packet registerPacket(const QUuid& sender, const QUuid& receiver, RegisterInfo info);
    static Packet loginPacket(const QUuid& sender, const QUuid& receiver, LoginInfo info);
    static Packet profileRequestPacket(const QUuid& sender, const QUuid& receiver);
    static Packet profileUpdatePacket(const QUuid& sender, const QUuid& receiver, ProfileUpdateInfo info);
    static Packet profileDataPacket(const QUuid& sender, const QUuid& receiver, ProfileInfo info);
    static Packet userInfoRequestPacket(const QUuid& sender, const QUuid& receiver, const QUuid& userId);
    static Packet userInfoDataPacket(const QUuid& sender, const QUuid& receiver, PublicUserInfo info);
    static Packet successPacket(const QUuid& sender, const QUuid& receiver, QString message);
    static Packet errorPacket(const QUuid& sender, const QUuid& receiver, QString message);

};

}
