#include <gtest/gtest.h>

#include <QByteArray>
#include <QString>
#include <QUuid>

#include "KeyStore.h"
#include "util.h"

using shared::util::isValidUsername;
using shared::util::normalizeUsername;

namespace {

const QUuid FirstPeer{"{11111111-1111-1111-1111-111111111111}"};
const QUuid SecondPeer{"{22222222-2222-2222-2222-222222222222}"};
const QUuid ServerPeer{};

} // namespace

TEST(UtilTests, NormalizeUsernameTrimsLowercasesAndRemovesSingleAtPrefix)
{
    EXPECT_EQ(normalizeUsername("  @Alice_42  "), QStringLiteral("alice_42"));
    EXPECT_EQ(normalizeUsername("Bob"), QStringLiteral("bob"));
    EXPECT_EQ(normalizeUsername("@@Team"), QStringLiteral("@team"));
}

TEST(UtilTests, ValidUsernameAcceptsAllowedBoundaryCases)
{
    EXPECT_TRUE(isValidUsername("ab"));
    EXPECT_TRUE(isValidUsername("alice_42"));
    EXPECT_TRUE(isValidUsername("a1234567890123456789"));
}

TEST(UtilTests, ValidUsernameRejectsInvalidCharactersAndLengths)
{
    EXPECT_FALSE(isValidUsername(""));
    EXPECT_FALSE(isValidUsername("a"));
    EXPECT_FALSE(isValidUsername("a12345678901234567890"));
    EXPECT_FALSE(isValidUsername("Alice"));
    EXPECT_FALSE(isValidUsername("alice-42"));
    EXPECT_FALSE(isValidUsername("alice 42"));
    EXPECT_FALSE(isValidUsername("@alice"));
}

TEST(UtilTests, NormalizedUsernameCanBeValidated)
{
    const QString username = normalizeUsername("  @Valid_User20  ");

    EXPECT_EQ(username, QStringLiteral("valid_user20"));
    EXPECT_TRUE(isValidUsername(username));
}

TEST(KeyStoreTests, GeneratesLocalKeyPair)
{
    shared::KeyStore& keyStore = shared::KeyStore::instance();
    keyStore.regenerate();

    EXPECT_TRUE(keyStore.hasLocalKeyPair());
    EXPECT_FALSE(keyStore.publicKey().isEmpty());
    EXPECT_FALSE(keyStore.privateKey().isEmpty());
}

TEST(KeyStoreTests, StoresPeerPublicKeysBySessionId)
{
    shared::KeyStore& keyStore = shared::KeyStore::instance();
    keyStore.clearPeerPublicKeys();

    keyStore.setPeerPublicKey(ServerPeer, QByteArray{"server-public-key"});
    keyStore.setPeerPublicKey(FirstPeer, QByteArray{"first-public-key"});
    keyStore.setPeerPublicKey(SecondPeer, QByteArray{"second-public-key"});

    ASSERT_TRUE(keyStore.hasPeerPublicKey(ServerPeer));
    ASSERT_TRUE(keyStore.hasPeerPublicKey(FirstPeer));
    ASSERT_TRUE(keyStore.hasPeerPublicKey(SecondPeer));
    EXPECT_EQ(keyStore.peerPublicKey(ServerPeer).value(), QByteArray{"server-public-key"});
    EXPECT_EQ(keyStore.peerPublicKey(FirstPeer).value(), QByteArray{"first-public-key"});
    EXPECT_EQ(keyStore.peerPublicKey(SecondPeer).value(), QByteArray{"second-public-key"});
}

TEST(KeyStoreTests, RemovesPeerPublicKeys)
{
    shared::KeyStore& keyStore = shared::KeyStore::instance();
    keyStore.clearPeerPublicKeys();

    keyStore.setPeerPublicKey(FirstPeer, QByteArray{"first-public-key"});
    keyStore.setPeerPublicKey(SecondPeer, QByteArray{"second-public-key"});

    keyStore.removePeerPublicKey(FirstPeer);

    EXPECT_FALSE(keyStore.hasPeerPublicKey(FirstPeer));
    EXPECT_TRUE(keyStore.hasPeerPublicKey(SecondPeer));

    keyStore.clearPeerPublicKeys();

    EXPECT_FALSE(keyStore.hasPeerPublicKey(SecondPeer));
    EXPECT_FALSE(keyStore.hasPeerPublicKey(FirstPeer));
}
