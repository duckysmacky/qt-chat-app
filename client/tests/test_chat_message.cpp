#include <gtest/gtest.h>

#include <QTime>
#include <QUuid>

#include "ChatMessage.h"

TEST(ChatMessageTests, IncomingMessageWithNullSenderUsesUnknownSender)
{
    const ChatMessage message{
        QUuid{"{11111111-1111-1111-1111-111111111111}"},
        false,
        "hello",
        QUuid{}
    };

    EXPECT_FALSE(message.isOwn());
    EXPECT_EQ(message.content(), QStringLiteral("hello"));
    EXPECT_EQ(message.sender(), QStringLiteral("Unknown"));
    EXPECT_EQ(message.status(), ChatMessage::Status::Loading);
    EXPECT_EQ(message.statusText(), QStringLiteral("Sending"));
    EXPECT_FALSE(message.isReceived());
}

TEST(ChatMessageTests, ReceivedMessageFormatsReceivedTimeAndReportsReceived)
{
    ChatMessage message{
        QUuid{"{11111111-1111-1111-1111-111111111111}"},
        false,
        "hello",
        QUuid{}
    };

    message.setTimeReceived(QTime{9, 5});
    message.setStatus(ChatMessage::Status::Received);

    EXPECT_TRUE(message.isReceived());
    EXPECT_EQ(message.formattedTimeReceived(), QStringLiteral("09:05"));
    EXPECT_EQ(message.statusText(), QStringLiteral("Received"));
}

TEST(ChatMessageTests, SentTimeIsFormattedWhenSet)
{
    ChatMessage message{
        QUuid{"{11111111-1111-1111-1111-111111111111}"},
        false,
        "hello",
        QUuid{}
    };

    message.setTimeSent(QTime{23, 7});

    EXPECT_EQ(message.formattedTimeSent(), QStringLiteral("23:07"));
    EXPECT_FALSE(message.isSent());
}

TEST(ChatMessageTests, StatusTextCoversDeliveryStates)
{
    ChatMessage message{
        QUuid{"{11111111-1111-1111-1111-111111111111}"},
        false,
        "hello",
        QUuid{}
    };

    message.setStatus(ChatMessage::Status::Sent);
    EXPECT_EQ(message.statusText(), QStringLiteral("Sent"));

    message.setStatus(ChatMessage::Status::Delivered);
    EXPECT_EQ(message.statusText(), QStringLiteral("Delivered"));

    message.setStatus(ChatMessage::Status::Read);
    EXPECT_EQ(message.statusText(), QStringLiteral("Read"));

    message.setStatus(ChatMessage::Status::Failed);
    EXPECT_EQ(message.statusText(), QStringLiteral("Failed"));
}