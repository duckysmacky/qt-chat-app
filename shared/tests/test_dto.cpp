#include <gtest/gtest.h>

#include <QByteArray>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QString>
#include <QTimeZone>
#include <QUuid>

#include "Message.h"
#include "OperationResult.h"
#include "dto/AuthInfo.h"
#include "dto/ChatInfo.h"
#include "dto/ChatsInfo.h"
#include "dto/CreateChatInfo.h"
#include "dto/ProfileInfo.h"
#include "dto/ProfileUpdateInfo.h"
#include "dto/PublicUserInfo.h"
#include "dto/UserInfoRequest.h"

using namespace shared;

namespace {

const QUuid UserId{"{11111111-1111-1111-1111-111111111111}"};
const QUuid OtherUserId{"{22222222-2222-2222-2222-222222222222}"};
const QUuid ChatId{"{33333333-3333-3333-3333-333333333333}"};
const QDateTime CreatedAt{{2026, 5, 14}, {10, 15, 30}, QTimeZone::UTC};

QByteArray objectJson(std::initializer_list<std::pair<QString, QJsonValue>> fields)
{
    QJsonObject object;
    for (const auto& [key, value] : fields)
        object.insert(key, value);

    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

} // namespace

TEST(DtoTests, LoginInfoRoundTripsAndRejectsMissingFields)
{
    const LoginInfo info{"alice", "password-hash"};

    const auto decoded = LoginInfo::deserialize(info.serialize());

    ASSERT_TRUE(decoded.has_value());
    EXPECT_EQ(decoded->login(), QStringLiteral("alice"));
    EXPECT_EQ(decoded->passwordHash(), QStringLiteral("password-hash"));
    EXPECT_FALSE(LoginInfo::deserialize(R"({"login":"alice"})").has_value());
    EXPECT_FALSE(LoginInfo::deserialize("[]").has_value());
}

TEST(DtoTests, RegisterInfoRoundTripsAndRejectsMissingFields)
{
    const RegisterInfo info{"alice", "Alice", "alice@example.com", "password-hash"};

    const auto decoded = RegisterInfo::deserialize(info.serialize());

    ASSERT_TRUE(decoded.has_value());
    EXPECT_EQ(decoded->username(), QStringLiteral("alice"));
    EXPECT_EQ(decoded->displayName(), QStringLiteral("Alice"));
    EXPECT_EQ(decoded->email(), QStringLiteral("alice@example.com"));
    EXPECT_EQ(decoded->passwordHash(), QStringLiteral("password-hash"));
    EXPECT_FALSE(RegisterInfo::deserialize(R"({"username":"alice"})").has_value());
}

TEST(DtoTests, ProfileInfoRoundTripsAndRejectsMissingFields)
{
    const ProfileInfo info{UserId, "alice", "Alice", "alice@example.com"};

    const auto decoded = ProfileInfo::deserialize(info.serialize());

    ASSERT_TRUE(decoded.has_value());
    EXPECT_EQ(decoded->userId(), UserId);
    EXPECT_EQ(decoded->username(), QStringLiteral("alice"));
    EXPECT_EQ(decoded->displayName(), QStringLiteral("Alice"));
    EXPECT_EQ(decoded->email(), QStringLiteral("alice@example.com"));
    EXPECT_FALSE(ProfileInfo::deserialize(R"({"userId":"11111111-1111-1111-1111-111111111111"})").has_value());
}

TEST(DtoTests, PublicUserInfoRoundTripsAndRejectsMissingFields)
{
    const PublicUserInfo info{UserId, "alice", "Alice"};

    const auto decoded = PublicUserInfo::deserialize(info.serialize());

    ASSERT_TRUE(decoded.has_value());
    EXPECT_EQ(decoded->userId(), UserId);
    EXPECT_EQ(decoded->username(), QStringLiteral("alice"));
    EXPECT_EQ(decoded->displayName(), QStringLiteral("Alice"));
    EXPECT_FALSE(PublicUserInfo::deserialize(R"({"username":"alice"})").has_value());
}

TEST(DtoTests, ProfileUpdateInfoSerializesOnlySetFields)
{
    ProfileUpdateInfo info;
    EXPECT_TRUE(info.isEmpty());
    EXPECT_FALSE(ProfileUpdateInfo::deserialize(info.serialize()).has_value());

    info.setUsername("alice2");
    info.setEmail("alice2@example.com");

    const auto decoded = ProfileUpdateInfo::deserialize(info.serialize());

    ASSERT_TRUE(decoded.has_value());
    EXPECT_FALSE(decoded->isEmpty());
    ASSERT_TRUE(decoded->username().has_value());
    EXPECT_EQ(decoded->username().value(), QStringLiteral("alice2"));
    EXPECT_FALSE(decoded->displayName().has_value());
    ASSERT_TRUE(decoded->email().has_value());
    EXPECT_EQ(decoded->email().value(), QStringLiteral("alice2@example.com"));
    EXPECT_FALSE(decoded->passwordHash().has_value());
}

TEST(DtoTests, UserInfoRequestRoundTripsUuidAndUsernameIdentifiers)
{
    const UserInfoRequest byId{UserId};
    const UserInfoRequest byUsername{QStringLiteral("alice")};

    const auto decodedId = UserInfoRequest::deserialize(byId.serialize());
    const auto decodedUsername = UserInfoRequest::deserialize(byUsername.serialize());

    ASSERT_TRUE(decodedId.has_value());
    EXPECT_EQ(decodedId->identifierType(), UserIdentifierType::UUID);
    EXPECT_EQ(decodedId->userId(), UserId);

    ASSERT_TRUE(decodedUsername.has_value());
    EXPECT_EQ(decodedUsername->identifierType(), UserIdentifierType::USERNAME);
    EXPECT_EQ(decodedUsername->username(), QStringLiteral("alice"));

    EXPECT_FALSE(UserInfoRequest::deserialize(R"({"identifierType":"email","username":"a@b.test"})").has_value());
    EXPECT_FALSE(UserInfoRequest::deserialize(R"({"identifierType":"uuid"})").has_value());
}

TEST(DtoTests, ChatCreateInfoKeepsValidMembersAndSkipsInvalidMembers)
{
    const QByteArray json = R"({"memberIds":[)"
        + QByteArray("\"") + UserId.toString(QUuid::WithoutBraces).toUtf8() + QByteArray("\",")
        + QByteArray("\"not-a-uuid\",17,")
        + QByteArray("\"") + OtherUserId.toString(QUuid::WithoutBraces).toUtf8() + QByteArray("\"")
        + QByteArray("]}");

    const auto decoded = ChatCreateInfo::deserialize(json);

    ASSERT_TRUE(decoded.has_value());
    ASSERT_EQ(decoded->memberIds().size(), 2);
    EXPECT_EQ(decoded->memberIds().at(0), UserId);
    EXPECT_EQ(decoded->memberIds().at(1), OtherUserId);
    EXPECT_FALSE(ChatCreateInfo::deserialize(R"({"memberIds":"not-array"})").has_value());
}

TEST(DtoTests, ChatInfoRoundTripsAndRejectsInvalidMemberIds)
{
    const ChatInfo info{ChatId, "group", UserId, CreatedAt, QList<QUuid>{UserId, OtherUserId}};

    const auto decoded = ChatInfo::deserialize(info.serialize());

    ASSERT_TRUE(decoded.has_value());
    EXPECT_EQ(decoded->id(), ChatId);
    EXPECT_EQ(decoded->type(), QStringLiteral("group"));
    EXPECT_EQ(decoded->createdBy(), UserId);
    EXPECT_EQ(decoded->createdAt(), CreatedAt);
    EXPECT_EQ(decoded->memberIds(), QList<QUuid>({UserId, OtherUserId}));

    EXPECT_FALSE(ChatInfo::deserialize(objectJson({
        {"id", ChatId.toString(QUuid::WithoutBraces)},
        {"type", "group"},
        {"createdBy", UserId.toString(QUuid::WithoutBraces)},
        {"createdAt", CreatedAt.toUTC().toString(Qt::ISODate)},
        {"memberIds", QJsonArray{"not-a-uuid"}}
    })).has_value());
}

TEST(DtoTests, ChatsInfoRoundTripsAListAndRejectsInvalidEntries)
{
    const ChatInfo first{ChatId, "direct", UserId, CreatedAt, QList<QUuid>{OtherUserId}};
    const ChatInfo second{OtherUserId, "group", OtherUserId, CreatedAt, QList<QUuid>{UserId, OtherUserId}};
    const ChatsInfo chats{QList<ChatInfo>{first, second}};

    const auto decoded = ChatsInfo::deserialize(chats.serialize());

    ASSERT_TRUE(decoded.has_value());
    ASSERT_EQ(decoded->chats().size(), 2);
    EXPECT_EQ(decoded->chats().at(0).id(), first.id());
    EXPECT_EQ(decoded->chats().at(1).memberIds(), second.memberIds());
    EXPECT_FALSE(ChatsInfo::deserialize(R"([{"id":"only-id"}])").has_value());
    EXPECT_FALSE(ChatsInfo::deserialize(R"({"not":"array"})").has_value());
}

TEST(DtoTests, MessageRoundTripsJsonAndSupportsLegacyPayload)
{
    const Message message{UserId, ChatId, MessageType::TEXT, "hello"};

    const Message decoded = Message::deserialize(message.serialize());

    EXPECT_EQ(decoded.senderUserId(), UserId);
    EXPECT_EQ(decoded.targetChatId(), ChatId);
    EXPECT_EQ(decoded.type(), MessageType::TEXT);
    EXPECT_EQ(decoded.content(), QStringLiteral("hello"));

    QByteArray legacy;
    legacy.append(static_cast<char>(MessageType::MEDIA));
    legacy.append("file-id");
    const Message legacyDecoded = Message::deserialize(legacy);
    EXPECT_EQ(legacyDecoded.type(), MessageType::MEDIA);
    EXPECT_EQ(legacyDecoded.content(), QStringLiteral("file-id"));
}

TEST(DtoTests, OperationResultRoundTripsAndEmptyPayloadBecomesError)
{
    const OperationResult result{OperationResultType::SUCCESS, "ok"};

    const OperationResult decoded = OperationResult::deserialize(result.serialize());
    const OperationResult empty = OperationResult::deserialize({});

    EXPECT_EQ(decoded.type(), OperationResultType::SUCCESS);
    EXPECT_EQ(decoded.text(), QStringLiteral("ok"));
    EXPECT_EQ(empty.type(), OperationResultType::ERROR);
    EXPECT_TRUE(empty.text().isEmpty());
}