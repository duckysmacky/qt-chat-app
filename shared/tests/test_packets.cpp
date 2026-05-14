#include <gtest/gtest.h>

#include <QByteArray>
#include <QList>
#include <QUuid>

#include "Packet.h"
#include "util.h"

using namespace shared;

namespace {

const QUuid Sender{"{11111111-1111-1111-1111-111111111111}"};
const QUuid Receiver{"{22222222-2222-2222-2222-222222222222}"};

static void expectSamePacket(const Packet& actual, const Packet& expected)
{
    EXPECT_EQ(actual.type(), expected.type());
    EXPECT_EQ(actual.sender(), expected.sender());
    EXPECT_EQ(actual.receiver(), expected.receiver());
    EXPECT_EQ(actual.data().has_value(), expected.data().has_value());

    if (expected.data().has_value())
        EXPECT_EQ(actual.data().value(), expected.data().value());
}

} // namespace

TEST(PacketTests, SerializeDeserializeRetainsHeaderAndData)
{
    const QByteArray data{"hello payload"};
    const Packet packet{PacketType::CHAT_MESSAGE, Sender, Receiver, data};

    const Packet deserialized = Packet::deserialize(packet.serialize());

    expectSamePacket(deserialized, packet);
}

TEST(PacketTests, SerializeDeserializeSupportsPacketsWithoutData)
{
    const Packet packet{PacketType::GET_CHATS, Sender, Receiver};

    const Packet deserialized = Packet::deserialize(packet.serialize());

    expectSamePacket(deserialized, packet);
}

TEST(PacketTests, DeserializeShortPayloadReturnsInvalidPacket)
{
    const Packet packet = Packet::deserialize(QByteArray{"too short"});

    EXPECT_EQ(packet.type(), PacketType::INVALID);
    EXPECT_TRUE(packet.sender().isNull());
    EXPECT_TRUE(packet.receiver().isNull());
    EXPECT_FALSE(packet.data().has_value());
}

TEST(PacketTests, EncapsulateEscapesDelimiterAndEscapeBytes)
{
    QByteArray data;
    data.append("before");
    data.append(util::DELIMITER);
    data.append("middle");
    data.append(util::ESCAPE);
    data.append("after");

    const Packet packet{PacketType::CHAT_MESSAGE, Sender, Receiver, data};
    const QByteArray encoded = util::encapsulate(packet);
    QByteArray escapedDelimiter;
    escapedDelimiter.append(util::ESCAPE);
    escapedDelimiter.append(util::ESCAPED_DELIMITER);
    QByteArray escapedEscape;
    escapedEscape.append(util::ESCAPE);
    escapedEscape.append(util::ESCAPED_ESCAPE);

    EXPECT_TRUE(encoded.endsWith(util::DELIMITER));
    EXPECT_TRUE(encoded.contains(escapedDelimiter));
    EXPECT_TRUE(encoded.contains(escapedEscape));

    const QList<Packet> parsed = util::parse(encoded);

    ASSERT_EQ(parsed.size(), 1);
    expectSamePacket(parsed.front(), packet);
}

TEST(PacketTests, ParseStreamConsumesCompletePacketsAndKeepsPartialPayload)
{
    const Packet first{PacketType::CONNECT_CLIENT, Sender, Receiver};
    const Packet second{PacketType::SERVER_COMMAND, Receiver, Sender, QByteArray{"shutdown"}};

    const QByteArray firstEncoded = util::encapsulate(first);
    const QByteArray secondEncoded = util::encapsulate(second);
    const QByteArray partialSecond = secondEncoded.left(secondEncoded.size() - 1);
    QByteArray stream = firstEncoded + partialSecond;

    const QList<Packet> parsed = util::parseStream(stream);

    ASSERT_EQ(parsed.size(), 1);
    expectSamePacket(parsed.front(), first);
    EXPECT_EQ(stream, partialSecond);
}

TEST(PacketTests, ParseIgnoresIncompletePayloadWithoutDelimiter)
{
    const Packet packet{PacketType::GET_CHATS, Sender, Receiver};
    QByteArray encoded = util::encapsulate(packet);
    encoded.chop(1);

    const QList<Packet> parsed = util::parse(encoded);

    EXPECT_TRUE(parsed.isEmpty());
}

TEST(PacketTests, ParsePreservesUnknownEscapeSequenceInPayload)
{
    QByteArray raw;
    raw.append(static_cast<char>(PacketType::SERVER_COMMAND));
    raw.append(Sender.toRfc4122());
    raw.append(Receiver.toRfc4122());
    raw.append("prefix");
    raw.append(util::ESCAPE);
    raw.append('\x7f');
    raw.append("suffix");
    raw.append(util::DELIMITER);

    const QList<Packet> parsed = util::parse(raw);

    ASSERT_EQ(parsed.size(), 1);
    ASSERT_TRUE(parsed.front().data().has_value());
    QByteArray expected{"prefix"};
    expected.append(util::ESCAPE);
    expected.append('\x7f');
    expected.append("suffix");
    EXPECT_EQ(parsed.front().data().value(), expected);
}