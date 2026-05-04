#include "PacketFactory.h"

#include "Message.h"
#include "OperationResult.h"
#include "dto/AuthInfo.h"
#include "dto/ProfileInfo.h"
#include "dto/ProfileUpdateInfo.h"
#include "dto/PublicUserInfo.h"
#include "dto/ChatInfo.h"
#include "dto/ChatsInfo.h"
#include "dto/CreateChatInfo.h"



namespace shared {

Packet PacketFactory::connectClientPacket(const QUuid& sender, const QUuid& receiver)
{
    return {PacketType::CONNECT_CLIENT, sender, receiver};
}


Packet PacketFactory::chatMessagePacket(const QUuid& sender, const QUuid& receiver, Message message)
{
    return Packet{PacketType::CHAT_MESSAGE, sender, receiver, message.serialize()};
}

Packet PacketFactory::textChatMessagePacket(const QUuid& sender, const QUuid& receiver, const QUuid& senderUserId, const QUuid& targetChatId, QString content)
{
    const Message message(senderUserId, targetChatId, MessageType::TEXT, std::move(content));
    return chatMessagePacket(sender, receiver, message);
}

Packet PacketFactory::mediaChatMessagePacket(const QUuid& sender, const QUuid& receiver, const QUuid& senderUserId, const QUuid& targetChatId, QString content)
{
    const Message message(senderUserId, targetChatId, MessageType::MEDIA, std::move(content));
    return chatMessagePacket(sender, receiver, message);
}

Packet PacketFactory::registerUserPacket(const QUuid& sender, const QUuid& receiver, RegisterInfo info)
{
    return Packet{PacketType::REGISTER_USER, sender, receiver, info.serialize()};
}
Packet PacketFactory::loginUserPacket(const QUuid& sender, const QUuid& receiver, LoginInfo info)
{
    return Packet{PacketType::LOGIN_USER, sender, receiver, info.serialize()};
}

Packet PacketFactory::getUserProfilePacket(const QUuid& sender, const QUuid& receiver)
{
    return Packet{PacketType::GET_USER_PROFILE, sender, receiver};
}

Packet PacketFactory::updateUserProfilePacket(const QUuid& sender, const QUuid& receiver, ProfileUpdateInfo info)
{
    return Packet{PacketType::UPDATE_USER_PROFILE, sender, receiver, info.serialize()};
}

Packet PacketFactory::userProfileDataPacket(const QUuid& sender, const QUuid& receiver, ProfileInfo info)
{
    return Packet{PacketType::USER_PROFILE_DATA, sender, receiver, info.serialize()};
}

Packet PacketFactory::getPublicUserInfoPacket(const QUuid& sender, const QUuid& receiver, const QUuid& userId)
{
    return Packet{
        PacketType::GET_PUBLIC_USER_INFO,
        sender,
        receiver,
        userId.toRfc4122()
    };
}

Packet PacketFactory::publicUserInfoDataPacket(const QUuid& sender, const QUuid& receiver, PublicUserInfo info)
{
    return Packet{PacketType::PUBLIC_USER_INFO_DATA, sender, receiver, info.serialize()};
}

Packet PacketFactory::operationSuccessPacket(const QUuid& sender, const QUuid& receiver, QString message)
{
    const OperationResult result(OperationResultType::SUCCESS, std::move(message));
    return Packet{PacketType::OPERATION_RESULT, sender, receiver, result.serialize()};
}

Packet PacketFactory::operationErrorPacket(const QUuid& sender, const QUuid& receiver, QString message)
{
    const OperationResult result(OperationResultType::ERROR, std::move(message));
    return Packet{PacketType::OPERATION_RESULT, sender, receiver, result.serialize()};
}

Packet PacketFactory::getChatsPacket(const QUuid& sender, const QUuid& receiver)
{
    return Packet{PacketType::GET_CHATS, sender, receiver};
}

Packet PacketFactory::chatListDataPacket(const QUuid& sender, const QUuid& receiver, ChatsInfo info)
{
    return Packet{PacketType::CHAT_LIST_DATA, sender, receiver, info.serialize()};
}

Packet PacketFactory::searchChatsPacket(const QUuid& sender, const QUuid& receiver, QString query)
{
    return Packet{PacketType::SEARCH_CHATS, sender, receiver, query.toUtf8()};
}

Packet PacketFactory::createChatPacket(const QUuid& sender, const QUuid& receiver, ChatCreateInfo info)
{
    return Packet{PacketType::CREATE_CHAT, sender, receiver, info.serialize()};
}

Packet PacketFactory::chatInfoDataPacket(const QUuid& sender, const QUuid& receiver, ChatInfo info)
{
    return Packet{PacketType::CHAT_INFO_DATA, sender, receiver, info.serialize()};
}


}
