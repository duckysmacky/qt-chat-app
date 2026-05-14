#include <gtest/gtest.h>

#include <QList>
#include <QString>
#include <QUuid>

#include "Message.h"
#include "OperationResult.h"
#include "PacketFactory.h"
#include "dto/AuthInfo.h"
#include "dto/CreateChatInfo.h"
#include "dto/ProfileUpdateInfo.h"
#include "dto/UserInfoRequest.h"

using namespace shared;

namespace {

const QUuid Sender{"{11111111-1111-1111-1111-111111111111}"};
const QUuid Receiver{"{22222222-2222-2222-2222-222222222222}"};
const QUuid UserId{"{33333333-3333-3333-3333-333333333333}"};
const QUuid ChatId{"{44444444-4444-4444-4444-444444444444}"};

void expectPacketHeader(const Packet& packet, PacketType type)
{
    EXPECT_EQ(packet.type(), type);
    EXPECT_EQ(packet.sender(), Sender);
    EXPECT_EQ(packet.receiver(), Receiver);
}

} // namespace

TEST(PacketFactoryTests, CreatesHeaderOnlyPackets)
{
    const Packet connect = PacketFactory::connectClientPacket(Sender, Receiver);
    const Packet getProfile = PacketFactory::getUserProfilePacket(Sender, Receiver);
    const Packet getChats = PacketFactory::getChatsPacket(Sender, Receiver);

    expectPacketHeader(connect, PacketType::CONNECT_CLIENT);
    EXPECT_FALSE(connect.data().has_value());

    expectPacketHeader(getProfile, PacketType::GET_USER_PROFILE);
    EXPECT_FALSE(getProfile.data().has_value());

    expectPacketHeader(getChats, PacketType::GET_CHATS);
    EXPECT_FALSE(getChats.data().has_value());
}

TEST(PacketFactoryTests, CreatesTextAndMediaChatMessagePackets)
{
    const Packet text = PacketFactory::textChatMessagePacket(Sender, Receiver, UserId, ChatId, "hello");
    const Packet media = PacketFactory::mediaChatMessagePacket(Sender, Receiver, UserId, ChatId, "file-id");

    expectPacketHeader(text, PacketType::CHAT_MESSAGE);
    ASSERT_TRUE(text.data().has_value());
    const Message textMessage = Message::deserialize(text.data().value());
    EXPECT_EQ(textMessage.senderUserId(), UserId);
    EXPECT_EQ(textMessage.targetChatId(), ChatId);
    EXPECT_EQ(textMessage.type(), MessageType::TEXT);
    EXPECT_EQ(textMessage.content(), QStringLiteral("hello"));

    expectPacketHeader(media, PacketType::CHAT_MESSAGE);
    ASSERT_TRUE(media.data().has_value());
    const Message mediaMessage = Message::deserialize(media.data().value());
    EXPECT_EQ(mediaMessage.type(), MessageType::MEDIA);
    EXPECT_EQ(mediaMessage.content(), QStringLiteral("file-id"));
}

TEST(PacketFactoryTests, CreatesAuthenticationPacketsWithSerializablePayloads)
{
    const Packet login = PacketFactory::loginUserPacket(
        Sender,
        Receiver,
        LoginInfo{"alice", "hash"}
    );
    const Packet registration = PacketFactory::registerUserPacket(
        Sender,
        Receiver,
        RegisterInfo{"alice", "Alice", "alice@example.com", "hash"}
    );

    expectPacketHeader(login, PacketType::LOGIN_USER);
    ASSERT_TRUE(login.data().has_value());
    const auto loginInfo = LoginInfo::deserialize(login.data().value());
    ASSERT_TRUE(loginInfo.has_value());
    EXPECT_EQ(loginInfo->login(), QStringLiteral("alice"));

    expectPacketHeader(registration, PacketType::REGISTER_USER);
    ASSERT_TRUE(registration.data().has_value());
    const auto registerInfo = RegisterInfo::deserialize(registration.data().value());
    ASSERT_TRUE(registerInfo.has_value());
    EXPECT_EQ(registerInfo->email(), QStringLiteral("alice@example.com"));
}

TEST(PacketFactoryTests, CreatesProfileUpdateAndUserInfoPackets)
{
    ProfileUpdateInfo updateInfo;
    updateInfo.setDisplayName("Alice Cooper");

    const Packet update = PacketFactory::updateUserProfilePacket(Sender, Receiver, updateInfo);
    const Packet byId = PacketFactory::getUserInfoPacket(Sender, Receiver, UserId);
    const Packet byUsername = PacketFactory::getUserInfoPacket(Sender, Receiver, QStringLiteral("alice"));

    expectPacketHeader(update, PacketType::UPDATE_USER_PROFILE);
    ASSERT_TRUE(update.data().has_value());
    const auto decodedUpdate = ProfileUpdateInfo::deserialize(update.data().value());
    ASSERT_TRUE(decodedUpdate.has_value());
    ASSERT_TRUE(decodedUpdate->displayName().has_value());
    EXPECT_EQ(decodedUpdate->displayName().value(), QStringLiteral("Alice Cooper"));

    expectPacketHeader(byId, PacketType::GET_USER_INFO);
    ASSERT_TRUE(byId.data().has_value());
    const auto decodedById = UserInfoRequest::deserialize(byId.data().value());
    ASSERT_TRUE(decodedById.has_value());
    EXPECT_EQ(decodedById->identifierType(), UserIdentifierType::UUID);
    EXPECT_EQ(decodedById->userId(), UserId);

    expectPacketHeader(byUsername, PacketType::GET_USER_INFO);
    ASSERT_TRUE(byUsername.data().has_value());
    const auto decodedByUsername = UserInfoRequest::deserialize(byUsername.data().value());
    ASSERT_TRUE(decodedByUsername.has_value());
    EXPECT_EQ(decodedByUsername->identifierType(), UserIdentifierType::USERNAME);
    EXPECT_EQ(decodedByUsername->username(), QStringLiteral("alice"));
}

TEST(PacketFactoryTests, CreatesOperationResultPackets)
{
    const Packet success = PacketFactory::operationSuccessPacket(Sender, Receiver, "done");
    const Packet error = PacketFactory::operationErrorPacket(Sender, Receiver, "failed");

    expectPacketHeader(success, PacketType::OPERATION_RESULT);
    ASSERT_TRUE(success.data().has_value());
    const OperationResult successResult = OperationResult::deserialize(success.data().value());
    EXPECT_EQ(successResult.type(), OperationResultType::SUCCESS);
    EXPECT_EQ(successResult.text(), QStringLiteral("done"));

    expectPacketHeader(error, PacketType::OPERATION_RESULT);
    ASSERT_TRUE(error.data().has_value());
    const OperationResult errorResult = OperationResult::deserialize(error.data().value());
    EXPECT_EQ(errorResult.type(), OperationResultType::ERROR);
    EXPECT_EQ(errorResult.text(), QStringLiteral("failed"));
}

TEST(PacketFactoryTests, CreatesSearchAndCreateChatPackets)
{
    const Packet search = PacketFactory::searchChatsPacket(Sender, Receiver, "project");
    const Packet create = PacketFactory::createChatPacket(
        Sender,
        Receiver,
        ChatCreateInfo{QList<QUuid>{UserId}}
    );

    expectPacketHeader(search, PacketType::SEARCH_CHATS);
    ASSERT_TRUE(search.data().has_value());
    EXPECT_EQ(QString::fromUtf8(search.data().value()), QStringLiteral("project"));

    expectPacketHeader(create, PacketType::CREATE_CHAT);
    ASSERT_TRUE(create.data().has_value());
    const auto createInfo = ChatCreateInfo::deserialize(create.data().value());
    ASSERT_TRUE(createInfo.has_value());
    EXPECT_EQ(createInfo->memberIds(), QList<QUuid>({UserId}));
}