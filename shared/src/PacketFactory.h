#pragma once

#include "Packet.h"
#include "AuthInfo.h"

namespace shared {

/**
 * @brief Utility factory for creating different packet types.
 */
class PacketFactory
{
public:
    /// Creates CONNECT packet.
    static Packet connectPacket(const QUuid& sender, const QUuid& target);

    /// Creates text message packet.
    static Packet textMessagePacket(const QUuid& sender, const QUuid& target, QString content);

    /// Creates media message packet.
    static Packet mediaMessagePacket(const QUuid& sender, const QUuid& target, QString content);

    /// Creates registration packet.
    static Packet registerPacket(const QUuid& sender, const QUuid& target, const RegisterInfo& info);

    /// Creates login packet.
    static Packet loginPacket(const QUuid& sender, const QUuid& target, const LoginInfo& info);

    /// Creates success result packet.
    static Packet successPacket(const QUuid& sender, const QUuid& target, QString message);

    /// Creates error result packet.
    static Packet errorPacket(const QUuid& sender, const QUuid& target, QString message);
};

}