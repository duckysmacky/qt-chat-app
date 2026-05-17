#include <gtest/gtest.h>

#include <QString>

#include "util.h"

using shared::util::isValidUsername;
using shared::util::normalizeUsername;

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