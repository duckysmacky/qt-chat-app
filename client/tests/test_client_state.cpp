#include <gtest/gtest.h>

#include <QString>
#include <QUuid>

#include "Client.h"
#include "UserResolver.h"

TEST(ClientStateTests, InvalidConnectionInputSetsStatusWithoutConnecting)
{
    Client& client = Client::instance();

    client.connectTo("", 0);

    EXPECT_FALSE(client.connected());
    EXPECT_EQ(client.statusText(), QStringLiteral("Invalid host or port"));
}

TEST(ClientStateTests, UpdateServerIdIgnoresNullAndStoresValidId)
{
    Client& client = Client::instance();
    const QUuid serverId{"{11111111-1111-1111-1111-111111111111}"};

    client.updateServerId(QUuid{});
    EXPECT_TRUE(client.serverId().isNull());

    client.updateServerId(serverId);
    EXPECT_EQ(client.serverId(), serverId);

    client.updateServerId(QUuid{});
    EXPECT_EQ(client.serverId(), serverId);
}

TEST(ClientStateTests, UserResolverReturnsEmptyWhenOfflineOrInputInvalid)
{
    UserResolver& resolver = UserResolver::instance();
    resolver.clearCache();

    EXPECT_FALSE(resolver.resolveUser(QUuid{}).has_value());
    EXPECT_FALSE(resolver.resolveUser(QString{}).has_value());
    EXPECT_FALSE(resolver.resolveUser(QStringLiteral("   ")).has_value());
    EXPECT_FALSE(resolver.resolveUser(QUuid{"{22222222-2222-2222-2222-222222222222}"}).has_value());

    resolver.invalidateUser(QUuid{"{22222222-2222-2222-2222-222222222222}"});
    resolver.invalidateUser(QStringLiteral("@alice"));
    resolver.clearCache();
}