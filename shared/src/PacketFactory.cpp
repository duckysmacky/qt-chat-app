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
#include "dto/UserInfoRequest.h"



namespace shared {

Packet PacketFactory::connectClientPacket(const QUuid& sender, const QUuid& receiver)
{
    return {PacketType::CONNECT_CLIENT, sender, receiver};
}


Packet PacketFactory::chatMessagePacket(const QUuid& sender, const QUuid& receiver, Message message, const QByteArray& encryptionKey)
{
    Packet packet{PacketType::CHAT_MESSAGE, sender, receiver};
    packet.setPayload(message.serialize(), encryptionKey);
    return packet;
}

Packet PacketFactory::registerUserPacket(const QUuid& sender, const QUuid& receiver, RegisterInfo info, const QByteArray& encryptionKey)
{
    Packet packet{PacketType::REGISTER_USER, sender, receiver};
    packet.setPayload(info.serialize(), encryptionKey);
    return packet;
}

Packet PacketFactory::loginUserPacket(const QUuid& sender, const QUuid& receiver, LoginInfo info, const QByteArray& encryptionKey)
{
    Packet packet{PacketType::LOGIN_USER, sender, receiver};
    packet.setPayload(info.serialize(), encryptionKey);
    return packet;
}

Packet PacketFactory::getUserProfilePacket(const QUuid& sender, const QUuid& receiver)
{
    return Packet{PacketType::GET_USER_PROFILE, sender, receiver};
}

Packet PacketFactory::updateUserProfilePacket(const QUuid& sender, const QUuid& receiver, ProfileUpdateInfo info, const QByteArray& encryptionKey)
{
    Packet packet{PacketType::UPDATE_USER_PROFILE, sender, receiver};
    packet.setPayload(info.serialize(), encryptionKey);
    return packet;
}

Packet PacketFactory::userProfileDataPacket(const QUuid& sender, const QUuid& receiver, ProfileInfo info, const QByteArray& encryptionKey)
{
    Packet packet{PacketType::USER_PROFILE_DATA, sender, receiver};
    packet.setPayload(info.serialize(), encryptionKey);
    return packet;
}

Packet PacketFactory::getUserInfoPacket(const QUuid& sender, const QUuid& receiver, UserInfoRequest request, const QByteArray& encryptionKey)
{
    Packet packet{PacketType::GET_USER_INFO, sender, receiver};
    packet.setPayload(request.serialize(), encryptionKey);
    return packet;
}

Packet PacketFactory::getUserInfoPacket(const QUuid& sender, const QUuid& receiver, const QUuid& userId, const QByteArray& encryptionKey)
{
    return getUserInfoPacket(sender, receiver, UserInfoRequest(userId), encryptionKey);
}

Packet PacketFactory::getUserInfoPacket(const QUuid& sender, const QUuid& receiver, QString username, const QByteArray& encryptionKey)
{
    return getUserInfoPacket(sender, receiver, UserInfoRequest(std::move(username)), encryptionKey);
}

Packet PacketFactory::publicUserInfoDataPacket(const QUuid& sender, const QUuid& receiver, PublicUserInfo info, const QByteArray& encryptionKey)
{
    Packet packet{PacketType::PUBLIC_USER_INFO_DATA, sender, receiver};
    packet.setPayload(info.serialize(), encryptionKey);
    return packet;
}

Packet PacketFactory::operationSuccessPacket(const QUuid& sender, const QUuid& receiver, QString message, const QByteArray& encryptionKey)
{
    const OperationResult result(OperationResultType::SUCCESS, std::move(message));
    Packet packet{PacketType::OPERATION_RESULT, sender, receiver};
    packet.setPayload(result.serialize(), encryptionKey);
    return packet;
}

Packet PacketFactory::operationErrorPacket(const QUuid& sender, const QUuid& receiver, QString message, const QByteArray& encryptionKey)
{
    const OperationResult result(OperationResultType::ERROR, std::move(message));
    Packet packet{PacketType::OPERATION_RESULT, sender, receiver};
    packet.setPayload(result.serialize(), encryptionKey);
    return packet;
}

Packet PacketFactory::getChatsPacket(const QUuid& sender, const QUuid& receiver)
{
    return Packet{PacketType::GET_CHATS, sender, receiver};
}

Packet PacketFactory::chatListDataPacket(const QUuid& sender, const QUuid& receiver, ChatsInfo info, const QByteArray& encryptionKey)
{
    Packet packet{PacketType::CHAT_LIST_DATA, sender, receiver};
    packet.setPayload(info.serialize(), encryptionKey);
    return packet;
}

Packet PacketFactory::searchChatsPacket(const QUuid& sender, const QUuid& receiver, QString query, const QByteArray& encryptionKey)
{
    Packet packet{PacketType::SEARCH_CHATS, sender, receiver};
    packet.setPayload(query.toUtf8(), encryptionKey);
    return packet;
}

Packet PacketFactory::createChatPacket(const QUuid& sender, const QUuid& receiver, ChatCreateInfo info, const QByteArray& encryptionKey)
{
    Packet packet{PacketType::CREATE_CHAT, sender, receiver};
    packet.setPayload(info.serialize(), encryptionKey);
    return packet;
}

Packet PacketFactory::chatInfoDataPacket(const QUuid& sender, const QUuid& receiver, ChatInfo info, const QByteArray& encryptionKey)
{
    Packet packet{PacketType::CHAT_INFO_DATA, sender, receiver};
    packet.setPayload(info.serialize(), encryptionKey);
    return packet;
}

Packet PacketFactory::keyExchangePacket(const QUuid& sender, const QUuid& receiver, QByteArray publicKey, const QByteArray& encryptionKey)
{
    Packet packet{PacketType::KEY_EXCHANGE, sender, receiver};
    packet.setPayload(publicKey, encryptionKey);
    return packet;
}

}