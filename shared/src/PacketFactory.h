#pragma once

#include "Packet.h"
#include "AuthInfo.h"

namespace shared {

/**
 * @class PacketFactory
 * @brief Factory class for creating various types of network packets.
 */
class PacketFactory
{
public:
    /**
     * @brief Creates a CONNECT packet for establishing a connection.
     * @param sender The UUID of the sender.
     * @param target The UUID of the target recipient.
     * @return A Packet object of type CONNECT.
     */
    static Packet connectPacket(const QUuid& sender, const QUuid& target);

    /**
     * @brief Creates a TEXT_MESSAGE packet for sending text messages.
     * @param sender The UUID of the sender.
     * @param target The UUID of the target recipient.
     * @param content The text content of the message.
     * @return A Packet object of type TEXT_MESSAGE.
     */
    static Packet textMessagePacket(const QUuid& sender, const QUuid& target, QString content);

    /**
     * @brief Creates a MEDIA_MESSAGE packet for sending media messages.
     * @param sender The UUID of the sender.
     * @param target The UUID of the target recipient.
     * @param content The media content or path.
     * @return A Packet object of type MEDIA_MESSAGE.
     */
    static Packet mediaMessagePacket(const QUuid& sender, const QUuid& target, QString content);

    /**
     * @brief Creates a REGISTER packet for user registration.
     * @param sender The UUID of the sender.
     * @param target The UUID of the target recipient.
     * @param info The registration information.
     * @return A Packet object of type REGISTER.
     */
    static Packet registerPacket(const QUuid& sender, const QUuid& target, const RegisterInfo& info);

    /**
     * @brief Creates a LOGIN packet for user authentication.
     * @param sender The UUID of the sender.
     * @param target The UUID of the target recipient.
     * @param info The login credentials.
     * @return A Packet object of type LOGIN.
     */
    static Packet loginPacket(const QUuid& sender, const QUuid& target, const LoginInfo& info);

    /**
     * @brief Creates a SUCCESS packet indicating a successful operation.
     * @param sender The UUID of the sender.
     * @param target The UUID of the target recipient.
     * @param message A success message.
     * @return A Packet object of type SUCCESS.
     */
    static Packet successPacket(const QUuid& sender, const QUuid& target, QString message);

    /**
     * @brief Creates an ERROR packet indicating an error condition.
     * @param sender The UUID of the sender.
     * @param target The UUID of the target recipient.
     * @param message An error message describing the issue.
     * @return A Packet object of type ERROR.
     */
    static Packet errorPacket(const QUuid& sender, const QUuid& target, QString message);
};

} // namespace shared