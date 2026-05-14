#include <gtest/gtest.h>

#include <QDateTime>
#include <QList>
#include <QString>
#include <QTimeZone>
#include <QUuid>

#include "RequestManager.h"
#include "Message.h"
#include "OperationResult.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "dto/ChatInfo.h"
#include "dto/ChatsInfo.h"
#include "dto/ProfileInfo.h"
#include "dto/PublicUserInfo.h"
#include "util.h"

namespace {

const QUuid ServerId{"{aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa}"};
const QUuid ClientId{"{bbbbbbbb-bbbb-bbbb-bbbb-bbbbbbbbbbbb}"};
const QUuid UserId{"{cccccccc-cccc-cccc-cccc-cccccccccccc}"};
const QUuid ChatId{"{dddddddd-dddd-dddd-dddd-dddddddddddd}"};

void processPacket(const shared::Packet& packet)
{
    RequestManager::instance().processBytes(shared::util::encapsulate(packet));
}

} // namespace

TEST(RequestManagerTests, RoutesChatMessagePackets)
{
    int receivedCount = 0;
    shared::Message received{shared::MessageType::TEXT, ""};
    auto connection = QObject::connect(
        &RequestManager::instance(),
        &RequestManager::chatMessageReceived,
        [&](const shared::Message& message) {
            receivedCount++;
            received = message;
        }
    );

    const shared::Message message{UserId, ChatId, shared::MessageType::TEXT, "hello"};
    processPacket(shared::PacketFactory::chatMessagePacket(ServerId, ClientId, message));
    QObject::disconnect(connection);

    EXPECT_EQ(receivedCount, 1);
    EXPECT_EQ(received.senderUserId(), UserId);
    EXPECT_EQ(received.targetChatId(), ChatId);
    EXPECT_EQ(received.content(), QStringLiteral("hello"));
}

TEST(RequestManagerTests, RoutesOperationResultPackets)
{
    int receivedCount = 0;
    shared::OperationResult received{shared::OperationResultType::ERROR, ""};
    auto connection = QObject::connect(
        &RequestManager::instance(),
        &RequestManager::operationResultReceived,
        [&](const shared::OperationResult& result) {
            receivedCount++;
            received = result;
        }
    );

    processPacket(shared::PacketFactory::operationSuccessPacket(ServerId, ClientId, "ok"));
    QObject::disconnect(connection);

    EXPECT_EQ(receivedCount, 1);
    EXPECT_EQ(received.type(), shared::OperationResultType::SUCCESS);
    EXPECT_EQ(received.text(), QStringLiteral("ok"));
}

TEST(RequestManagerTests, RoutesProfileAndPublicUserInfoPackets)
{
    int profileCount = 0;
    int publicInfoCount = 0;
    shared::ProfileInfo profile;
    shared::PublicUserInfo publicInfo;
    auto profileConnection = QObject::connect(
        &RequestManager::instance(),
        &RequestManager::currentUserProfileReceived,
        [&](const shared::ProfileInfo& info) {
            profileCount++;
            profile = info;
        }
    );
    auto publicInfoConnection = QObject::connect(
        &RequestManager::instance(),
        &RequestManager::publicUserInfoReceived,
        [&](const shared::PublicUserInfo& info) {
            publicInfoCount++;
            publicInfo = info;
        }
    );

    processPacket(shared::PacketFactory::userProfileDataPacket(
        ServerId,
        ClientId,
        shared::ProfileInfo{UserId, "alice", "Alice", "alice@example.com"}
    ));
    processPacket(shared::PacketFactory::publicUserInfoDataPacket(
        ServerId,
        ClientId,
        shared::PublicUserInfo{UserId, "alice", "Alice"}
    ));
    QObject::disconnect(profileConnection);
    QObject::disconnect(publicInfoConnection);

    EXPECT_EQ(profileCount, 1);
    EXPECT_EQ(profile.userId(), UserId);
    EXPECT_EQ(profile.email(), QStringLiteral("alice@example.com"));
    EXPECT_EQ(publicInfoCount, 1);
    EXPECT_EQ(publicInfo.username(), QStringLiteral("alice"));
}

TEST(RequestManagerTests, RoutesChatListAndChatInfoPackets)
{
    const QDateTime createdAt{{2026, 5, 14}, {11, 0, 0}, QTimeZone::UTC};
    int chatListCount = 0;
    int chatInfoCount = 0;
    shared::ChatsInfo chats;
    shared::ChatInfo chat;
    auto chatListConnection = QObject::connect(
        &RequestManager::instance(),
        &RequestManager::chatListReceived,
        [&](const shared::ChatsInfo& info) {
            chatListCount++;
            chats = info;
        }
    );
    auto chatInfoConnection = QObject::connect(
        &RequestManager::instance(),
        &RequestManager::chatInfoReceived,
        [&](const shared::ChatInfo& info) {
            chatInfoCount++;
            chat = info;
        }
    );

    const shared::ChatInfo chatInfo{ChatId, "group", UserId, createdAt, QList<QUuid>{UserId}};
    processPacket(shared::PacketFactory::chatListDataPacket(
        ServerId,
        ClientId,
        shared::ChatsInfo{QList<shared::ChatInfo>{chatInfo}}
    ));
    processPacket(shared::PacketFactory::chatInfoDataPacket(ServerId, ClientId, chatInfo));
    QObject::disconnect(chatListConnection);
    QObject::disconnect(chatInfoConnection);

    EXPECT_EQ(chatListCount, 1);
    ASSERT_EQ(chats.chats().size(), 1);
    EXPECT_EQ(chats.chats().front().id(), ChatId);
    EXPECT_EQ(chatInfoCount, 1);
    EXPECT_EQ(chat.id(), ChatId);
}

TEST(RequestManagerTests, EmitsInvalidAndUnsupportedPacketSignals)
{
    int invalidCount = 0;
    int unsupportedCount = 0;
    auto invalidConnection = QObject::connect(
        &RequestManager::instance(),
        &RequestManager::invalidPacketReceived,
        [&](const shared::Packet&) { invalidCount++; }
    );
    auto unsupportedConnection = QObject::connect(
        &RequestManager::instance(),
        &RequestManager::unsupportedPacketReceived,
        [&](const shared::Packet&) { unsupportedCount++; }
    );

    processPacket(shared::Packet{shared::PacketType::INVALID, ServerId, ClientId});
    processPacket(shared::Packet{shared::PacketType::GET_CHATS, ServerId, ClientId});
    QObject::disconnect(invalidConnection);
    QObject::disconnect(unsupportedConnection);

    EXPECT_EQ(invalidCount, 1);
    EXPECT_EQ(unsupportedCount, 1);
}

TEST(RequestManagerTests, DoesNotEmitForInvalidTypedPayloads)
{
    int profileCount = 0;
    int publicInfoCount = 0;
    auto profileConnection = QObject::connect(
        &RequestManager::instance(),
        &RequestManager::currentUserProfileReceived,
        [&](const shared::ProfileInfo&) { profileCount++; }
    );
    auto publicInfoConnection = QObject::connect(
        &RequestManager::instance(),
        &RequestManager::publicUserInfoReceived,
        [&](const shared::PublicUserInfo&) { publicInfoCount++; }
    );

    processPacket(shared::Packet{shared::PacketType::USER_PROFILE_DATA, ServerId, ClientId, QByteArray{"not-json"}});
    processPacket(shared::Packet{shared::PacketType::PUBLIC_USER_INFO_DATA, ServerId, ClientId, QByteArray{"{}"}});
    QObject::disconnect(profileConnection);
    QObject::disconnect(publicInfoConnection);

    EXPECT_EQ(profileCount, 0);
    EXPECT_EQ(publicInfoCount, 0);
}