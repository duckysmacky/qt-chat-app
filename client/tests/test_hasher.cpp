#include <gtest/gtest.h>

#include <QRegularExpression>
#include <QString>

#include "Hasher.h"

TEST(HasherTests, Sha256ReturnsKnownDigestForEmptyString)
{
    EXPECT_EQ(
        Hasher::sha256(""),
        QStringLiteral("e3b0c44298fc1c149afbf4c8996fb924"
                       "27ae41e4649b934ca495991b7852b855")
    );
}

TEST(HasherTests, Sha256ReturnsKnownDigestForPassword)
{
    EXPECT_EQ(
        Hasher::sha256("password"),
        QStringLiteral("5e884898da28047151d0e56f8dc62927"
                       "73603d0d6aabbdd62a11ef721d1542d8")
    );
}

TEST(HasherTests, Sha256IsDeterministicAndInputSensitive)
{
    const QString first = Hasher::sha256("chat-app");
    const QString repeated = Hasher::sha256("chat-app");
    const QString different = Hasher::sha256("Chat-App");

    EXPECT_EQ(first, repeated);
    EXPECT_NE(first, different);
}

TEST(HasherTests, Sha256ReturnsLowercaseHexDigest)
{
    const QString digest = Hasher::sha256("unicode: пароль");
    const QRegularExpression lowercaseSha256{"^[0-9a-f]{64}$"};

    EXPECT_TRUE(lowercaseSha256.match(digest).hasMatch());
}