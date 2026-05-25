/**
 * @file PacketFactory.h
 * @brief Definition of the PacketFactory class for creating network packets.
 */

#pragma once

#include "Packet.h"
#include "Message.h"
#include "dto/AuthInfo.h"
#include "dto/ProfileInfo.h"
#include "dto/ProfileUpdateInfo.h"
#include "dto/ChatInfo.h"
#include "dto/ChatsInfo.h"
#include "dto/CreateChatInfo.h"
#include "dto/UserInfoRequest.h"
#include "dto/PublicUserInfo.h"
#include "dto/SessionInfo.h"
#include "dto/ChatKeyInfo.h"
#include "dto/StoredChatKeyInfo.h"

namespace shared {

/**
 * @class PacketFactory
 * @brief Factory class for creating various types of network packets.
 *
 * The PacketFactory provides static methods to construct different types of
 * network packets used for communication between client and server. Each method
 * returns a properly formatted Packet object with appropriate packet types and
 * serialized payloads.
 *
 * This class follows the factory design pattern, centralizing packet creation
 * logic and ensuring consistent packet formatting across the application.
 */
class PacketFactory
{
public:
    /**
     * @brief Creates a CONNECT packet for initial handshake.
     * @param sender UUID of the sender (client).
     * @param receiver UUID of the receiver (server).
     * @return Packet configured as a client connection request.
     */
    static Packet connectPacket(const QUuid& sender, const QUuid& receiver);

    /**
     * @brief Creates a CHAT_MESSAGE packet containing a complete message.
     * @param sender UUID of the sender (client).
     * @param receiver UUID of the receiver (target chat or server).
     * @param message The Message object to send.
     * @return Packet containing the chat message.
     */
    static Packet chatMessagePacket(const QUuid& sender, const QUuid& receiver, Message message, const QByteArray& encryptionKey);

    /**
     * @brief Creates a REGISTER_USER packet for new user registration.
     * @param sender UUID of the sender (client).
     * @param receiver UUID of the receiver (server).
     * @param info Registration information containing user details.
     * @return Packet containing user registration data.
     */
    static Packet registerUserPacket(const QUuid& sender, const QUuid& receiver, RegisterInfo info, const QByteArray& encryptionKey);

    /**
     * @brief Creates a LOGIN_USER packet for user authentication.
     * @param sender UUID of the sender (client).
     * @param receiver UUID of the receiver (server).
     * @param info Login credentials (username/email and password hash).
     * @return Packet containing login authentication data.
     */
    static Packet loginUserPacket(const QUuid& sender, const QUuid& receiver, LoginInfo info, const QByteArray& encryptionKey);

    /**
     * @brief Creates a GET_USER_PROFILE packet to request the current user's profile.
     * @param sender UUID of the sender (client).
     * @param receiver UUID of the receiver (server).
     * @return Packet requesting user profile data.
     */
    static Packet getUserProfilePacket(const QUuid& sender, const QUuid& receiver);

    /**
     * @brief Creates an UPDATE_USER_PROFILE packet to modify user profile.
     * @param sender UUID of the sender (client).
     * @param receiver UUID of the receiver (server).
     * @param info Profile update information containing fields to modify.
     * @return Packet containing profile update data.
     */
    static Packet updateUserProfilePacket(const QUuid& sender, const QUuid& receiver, ProfileUpdateInfo info, const QByteArray& encryptionKey);

    /**
     * @brief Creates a USER_PROFILE_DATA packet to send profile information.
     * @param sender UUID of the sender (server).
     * @param receiver UUID of the receiver (client).
     * @param info The profile information to send.
     * @return Packet containing user profile data.
     */
    static Packet userProfileDataPacket(const QUuid& sender, const QUuid& receiver, ProfileInfo info, const QByteArray& encryptionKey);

    /**
     * @brief Creates a GET_USER_INFO packet with a request object.
     * @param sender UUID of the sender.
     * @param receiver UUID of the receiver.
     * @param request The UserInfoRequest containing search/filter criteria.
     * @return Packet requesting user information with specified criteria.
     */
    static Packet getUserInfoPacket(const QUuid& sender, const QUuid& receiver, UserInfoRequest request, const QByteArray& encryptionKey);

    /**
     * @brief Creates a GET_USER_INFO packet targeting a specific user ID.
     * @param sender UUID of the sender.
     * @param receiver UUID of the receiver.
     * @param userId UUID of the requested user.
     * @return Packet requesting user information for a specific user ID.
     */
    static Packet getUserInfoPacket(const QUuid& sender, const QUuid& receiver, const QUuid& userId, const QByteArray& encryptionKey);

    /**
     * @brief Creates a GET_USER_INFO packet targeting a specific username.
     * @param sender UUID of the sender.
     * @param receiver UUID of the receiver.
     * @param username Username of the requested user.
     * @return Packet requesting user information for a specific username.
     */
    static Packet getUserInfoPacket(const QUuid& sender, const QUuid& receiver, QString username, const QByteArray& encryptionKey);

    /**
     * @brief Creates a PUBLIC_USER_INFO_DATA packet with public user information.
     * @param sender UUID of the sender.
     * @param receiver UUID of the receiver.
     * @param info The public user information to send.
     * @return Packet containing public user data.
     */
    static Packet publicUserInfoDataPacket(const QUuid& sender, const QUuid& receiver, PublicUserInfo info, const QByteArray& encryptionKey);

    static Packet getUserSessionPacket(const QUuid& sender, const QUuid& receiver, const QUuid& userId, const QByteArray& encryptionKey);

    static Packet userSessionDataPacket(const QUuid& sender, const QUuid& receiver, SessionInfo info, const QByteArray& encryptionKey);

    /**
     * @brief Creates an OPERATION_SUCCESS packet indicating successful operation.
     * @param sender UUID of the sender.
     * @param receiver UUID of the receiver.
     * @param message Success message text.
     * @return Packet indicating operation success.
     */
    static Packet operationSuccessPacket(const QUuid& sender, const QUuid& receiver, QString message, const QByteArray& encryptionKey);

    /**
     * @brief Creates an OPERATION_ERROR packet indicating operation failure.
     * @param sender UUID of the sender.
     * @param receiver UUID of the receiver.
     * @param message Error message text.
     * @return Packet indicating operation failure.
     */
    static Packet operationErrorPacket(const QUuid& sender, const QUuid& receiver, QString message, const QByteArray& encryptionKey);

    /**
     * @brief Creates a GET_CHATS packet to request the user's chat list.
     * @param sender UUID of the sender (client).
     * @param receiver UUID of the receiver (server).
     * @return Packet requesting the list of chats.
     */
    static Packet getChatsPacket(const QUuid& sender, const QUuid& receiver);

    /**
     * @brief Creates a CHAT_LIST_DATA packet with chat list information.
     * @param sender UUID of the sender (server).
     * @param receiver UUID of the receiver (client).
     * @param info The ChatsInfo object containing chat list data.
     * @return Packet containing chat list data.
     */
    static Packet chatListDataPacket(const QUuid& sender, const QUuid& receiver, ChatsInfo info, const QByteArray& encryptionKey);

    /**
     * @brief Creates a SEARCH_CHATS packet to search for chats.
     * @param sender UUID of the sender (client).
     * @param receiver UUID of the receiver (server).
     * @param query The search query string.
     * @return Packet containing chat search criteria.
     */
    static Packet searchChatsPacket(const QUuid& sender, const QUuid& receiver, QString query, const QByteArray& encryptionKey);

    /**
     * @brief Creates a CREATE_CHAT packet to create a new chat.
     * @param sender UUID of the sender (client).
     * @param receiver UUID of the receiver (server).
     * @param info Chat creation information including members and chat type.
     * @return Packet containing chat creation data.
     */
    static Packet createChatPacket(const QUuid& sender, const QUuid& receiver, ChatCreateInfo info, const QByteArray& encryptionKey);

    /**
     * @brief Creates a CHAT_INFO_DATA packet with detailed chat information.
     * @param sender UUID of the sender.
     * @param receiver UUID of the receiver.
     * @param info The ChatInfo object containing detailed chat data.
     * @return Packet containing detailed chat information.
     */
    static Packet chatInfoDataPacket(const QUuid& sender, const QUuid& receiver, ChatInfo info, const QByteArray& encryptionKey);

    /**
     * @brief Creates a KEY_EXCHANGE packet containing a public key.
     * @param sender UUID of the sender.
     * @param receiver UUID of the receiver.
     * @param publicKey Public key bytes.
     * @return Packet containing the sender's public key.
     */
    static Packet publicKeyExchangePacket(const QUuid& sender, const QUuid& receiver, QByteArray publicKey, const QByteArray& encryptionKey);

    static Packet chatKeyExchangePacket(const QUuid& sender, const QUuid& receiver, ChatKeyInfo info, const QByteArray& encryptionKey);

    static Packet storeChatKeyPacket(const QUuid& sender, const QUuid& receiver, StoredChatKeyInfo info, const QByteArray& encryptionKey);
    static Packet getChatKeysPacket(const QUuid& sender, const QUuid& receiver);
    static Packet chatKeysDataPacket(const QUuid& sender, const QUuid& receiver, ChatKeysInfo info, const QByteArray& encryptionKey);
};

} // namespace shared
