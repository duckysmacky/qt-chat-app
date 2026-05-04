#pragma once

#include "Packet.h"
#include "Message.h"
#include "dto/AuthInfo.h"
#include "dto/ProfileInfo.h"
#include "dto/ProfileUpdateInfo.h"
#include "dto/ChatInfo.h"
#include "dto/ChatsInfo.h"
#include "dto/CreateChatInfo.h"

#include "dto/PublicUserInfo.h"


namespace shared {

/**
 * @class PacketFactory
 * @brief Factory class for creating various types of network packets.
 */
class PacketFactory
{
public:
    static Packet connectClientPacket(const QUuid& sender, const QUuid& receiver);
    static Packet chatMessagePacket(const QUuid& sender, const QUuid& receiver, Message message);
    static Packet textChatMessagePacket(const QUuid& sender, const QUuid& receiver, const QUuid& senderUserId, const QUuid& targetChatId, QString content);
    static Packet mediaChatMessagePacket(const QUuid& sender, const QUuid& receiver, const QUuid& senderUserId, const QUuid& targetChatId, QString content);
    static Packet registerUserPacket(const QUuid& sender, const QUuid& receiver, RegisterInfo info);
    static Packet loginUserPacket(const QUuid& sender, const QUuid& receiver, LoginInfo info);
    static Packet getUserProfilePacket(const QUuid& sender, const QUuid& receiver);
    static Packet updateUserProfilePacket(const QUuid& sender, const QUuid& receiver, ProfileUpdateInfo info);
    static Packet userProfileDataPacket(const QUuid& sender, const QUuid& receiver, ProfileInfo info);
    static Packet getPublicUserInfoPacket(const QUuid& sender, const QUuid& receiver, const QUuid& userId);
    static Packet publicUserInfoDataPacket(const QUuid& sender, const QUuid& receiver, PublicUserInfo info);
    static Packet operationSuccessPacket(const QUuid& sender, const QUuid& receiver, QString message);
    static Packet operationErrorPacket(const QUuid& sender, const QUuid& receiver, QString message);
    static Packet getChatsPacket(const QUuid& sender, const QUuid& receiver);
    static Packet chatListDataPacket(const QUuid& sender, const QUuid& receiver, ChatsInfo info);
    static Packet searchChatsPacket(const QUuid& sender, const QUuid& receiver, QString query);
    static Packet createChatPacket(const QUuid& sender, const QUuid& receiver, ChatCreateInfo info);
    static Packet chatInfoDataPacket(const QUuid& sender, const QUuid& receiver, ChatInfo info);


};

} // namespace shared
