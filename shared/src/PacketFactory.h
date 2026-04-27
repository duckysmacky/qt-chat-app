#pragma once

#include "Packet.h"
#include "Message.h"
#include "AuthInfo.h"
#include "ProfileInfo.h"
#include "ProfileUpdateInfo.h"
#include "PublicUserInfo.h"

namespace shared {

class PacketFactory
{
public:
    static Packet connectPacket(const QUuid& sender, const QUuid& target);
    static Packet messagePacket(const QUuid& sender, const QUuid& target, const Message& message);
    static Packet textMessagePacket(const QUuid& sender, const QUuid& target, QString content);
    static Packet mediaMessagePacket(const QUuid& sender, const QUuid& target, QString content);
    static Packet registerPacket(const QUuid& sender, const QUuid& target, const RegisterInfo& info);
    static Packet loginPacket(const QUuid& sender, const QUuid& target, const LoginInfo& info);
    static Packet profileRequestPacket(const QUuid& sender, const QUuid& target);
    static Packet profileUpdatePacket(const QUuid& sender, const QUuid& target, const ProfileUpdateInfo& info);
    static Packet profileDataPacket(const QUuid& sender, const QUuid& target, const ProfileInfo& info);
    static Packet userInfoRequestPacket(const QUuid& sender, const QUuid& target, const QUuid& userId);
    static Packet userInfoDataPacket(const QUuid& sender, const QUuid& target, const PublicUserInfo& info);
    static Packet successPacket(const QUuid& sender, const QUuid& target, QString message);
    static Packet errorPacket(const QUuid& sender, const QUuid& target, QString message);
};

}
