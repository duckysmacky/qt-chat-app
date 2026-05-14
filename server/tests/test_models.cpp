#include <gtest/gtest.h>

#include <QDateTime>
#include <QString>
#include <QTimeZone>
#include <QUuid>

#include "model/Chat.h"
#include "model/ChatMember.h"
#include "model/Content.h"
#include "model/DbMessage.h"
#include "model/User.h"
#include "model/UserStats.h"

using namespace model;

TEST(ModelTests, ChatTypeConvertsToAndFromDatabaseStrings)
{
    EXPECT_EQ(chatTypeToString(ChatType::Direct), QStringLiteral("direct"));
    EXPECT_EQ(chatTypeToString(ChatType::Group), QStringLiteral("group"));

    EXPECT_EQ(chatTypeFromString("direct"), ChatType::Direct);
    EXPECT_EQ(chatTypeFromString("group"), ChatType::Group);
    EXPECT_EQ(chatTypeFromString("unexpected"), ChatType::Direct);
}

TEST(ModelTests, UserConstructorAndSettersExposeStoredValues)
{
    User user{"alice", "Alice", "hash", "alice@example.com"};
    const QUuid id{"{aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa}"};

    EXPECT_FALSE(user.id().isNull());
    EXPECT_EQ(user.username(), QStringLiteral("alice"));
    EXPECT_EQ(user.displayName(), QStringLiteral("Alice"));
    EXPECT_EQ(user.passwordHash(), QStringLiteral("hash"));
    EXPECT_EQ(user.email(), QStringLiteral("alice@example.com"));

    user.setId(id);
    user.setUsername("bob");
    user.setDisplayName("Bob");
    user.setPasswordHash("new-hash");
    user.setEmail("bob@example.com");

    EXPECT_EQ(user.id(), id);
    EXPECT_EQ(user.username(), QStringLiteral("bob"));
    EXPECT_EQ(user.displayName(), QStringLiteral("Bob"));
    EXPECT_EQ(user.passwordHash(), QStringLiteral("new-hash"));
    EXPECT_EQ(user.email(), QStringLiteral("bob@example.com"));
}

TEST(ModelTests, ChatConstructorAndSettersExposeStoredValues)
{
    const QUuid creator{"{11111111-1111-1111-1111-111111111111}"};
    Chat chat{ChatType::Group, creator};

    EXPECT_FALSE(chat.id().isNull());
    EXPECT_EQ(chat.type(), ChatType::Group);
    EXPECT_EQ(chat.createdBy(), creator);
    EXPECT_TRUE(chat.createdAt().isValid());

    const QUuid id{"{22222222-2222-2222-2222-222222222222}"};
    const QUuid newCreator{"{33333333-3333-3333-3333-333333333333}"};
    const QDateTime createdAt{{2026, 5, 14}, {12, 30, 0}, QTimeZone::UTC};

    chat.setId(id);
    chat.setType(ChatType::Direct);
    chat.setCreatedBy(newCreator);
    chat.setCreatedAt(createdAt);

    EXPECT_EQ(chat.id(), id);
    EXPECT_EQ(chat.type(), ChatType::Direct);
    EXPECT_EQ(chat.createdBy(), newCreator);
    EXPECT_EQ(chat.createdAt(), createdAt);
}

TEST(ModelTests, ContentConstructorAndSettersExposeStoredValues)
{
    Content content{"hello", "image.png", 128.5};

    EXPECT_FALSE(content.id().isNull());
    EXPECT_EQ(content.content(), QStringLiteral("hello"));
    EXPECT_EQ(content.file(), QStringLiteral("image.png"));
    EXPECT_DOUBLE_EQ(content.fileSize(), 128.5);

    const QUuid id{"{44444444-4444-4444-4444-444444444444}"};
    content.setId(id);
    content.setContent("updated");
    content.setFile("video.mp4");
    content.setFileSize(2048.0);

    EXPECT_EQ(content.id(), id);
    EXPECT_EQ(content.content(), QStringLiteral("updated"));
    EXPECT_EQ(content.file(), QStringLiteral("video.mp4"));
    EXPECT_DOUBLE_EQ(content.fileSize(), 2048.0);
}

TEST(ModelTests, ChatMemberConstructorAndSettersExposeStoredValues)
{
    const QUuid chatId{"{55555555-5555-5555-5555-555555555555}"};
    const QUuid userId{"{66666666-6666-6666-6666-666666666666}"};
    ChatMember member{chatId, userId};

    EXPECT_EQ(member.chatId(), chatId);
    EXPECT_EQ(member.userId(), userId);

    const QUuid newChatId{"{77777777-7777-7777-7777-777777777777}"};
    const QUuid newUserId{"{88888888-8888-8888-8888-888888888888}"};
    member.setChatId(newChatId);
    member.setUserId(newUserId);

    EXPECT_EQ(member.chatId(), newChatId);
    EXPECT_EQ(member.userId(), newUserId);
}

TEST(ModelTests, DbMessageConstructorAndSettersExposeStoredValues)
{
    const QUuid toId{"{11111111-2222-3333-4444-555555555555}"};
    const QUuid fromId{"{22222222-3333-4444-5555-666666666666}"};
    const QUuid contentId{"{33333333-4444-5555-6666-777777777777}"};
    const QUuid chatId{"{44444444-5555-6666-7777-888888888888}"};
    DbMessage message{toId, fromId, contentId, chatId};

    EXPECT_FALSE(message.id().isNull());
    EXPECT_EQ(message.toId(), toId);
    EXPECT_EQ(message.fromId(), fromId);
    EXPECT_EQ(message.contentId(), contentId);
    EXPECT_EQ(message.chatId(), chatId);
    EXPECT_TRUE(message.createdAt().isValid());

    const QUuid id{"{99999999-9999-9999-9999-999999999999}"};
    const QDateTime createdAt{{2026, 5, 14}, {13, 0, 0}, QTimeZone::UTC};
    const QDateTime updatedAt{{2026, 5, 14}, {13, 5, 0}, QTimeZone::UTC};

    message.setId(id);
    message.setToId(fromId);
    message.setFromId(toId);
    message.setContentId(chatId);
    message.setChatId(contentId);
    message.setCreatedAt(createdAt);
    message.setUpdatedAt(updatedAt);

    EXPECT_EQ(message.id(), id);
    EXPECT_EQ(message.toId(), fromId);
    EXPECT_EQ(message.fromId(), toId);
    EXPECT_EQ(message.contentId(), chatId);
    EXPECT_EQ(message.chatId(), contentId);
    EXPECT_EQ(message.createdAt(), createdAt);
    EXPECT_EQ(message.updatedAt(), updatedAt);
}

TEST(ModelTests, UserStatsConstructorAndSettersExposeStoredValues)
{
    const QUuid userId{"{aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee}"};
    model::UserStats stats{userId, 5, 2, QStringList{"old_alice", "older_alice"}};

    EXPECT_EQ(stats.userId(), userId);
    EXPECT_EQ(stats.messagesSent(), 5);
    EXPECT_EQ(stats.chatsJoined(), 2);
    EXPECT_EQ(stats.exUsernames(), QStringList({"old_alice", "older_alice"}));

    const QUuid newUserId{"{ffffffff-eeee-dddd-cccc-bbbbbbbbbbbb}"};
    stats.setUserId(newUserId);
    stats.setMessagesSent(8);
    stats.setChatsJoined(4);
    stats.setExUsernames(QStringList{"alice", "alice2"});

    EXPECT_EQ(stats.userId(), newUserId);
    EXPECT_EQ(stats.messagesSent(), 8);
    EXPECT_EQ(stats.chatsJoined(), 4);
    EXPECT_EQ(stats.exUsernames(), QStringList({"alice", "alice2"}));
}