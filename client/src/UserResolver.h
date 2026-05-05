#pragma once

#include <QHash>
#include <QObject>
#include <QSet>
#include <QUuid>

#include <optional>

#include "dto/PublicUserInfo.h"

/**
 * @class UserResolver
 * @brief Resolves public user data by user ID and caches resolved users.
 */
class UserResolver : public QObject
{
    Q_OBJECT

private:
    QHash<QUuid, shared::PublicUserInfo> m_cache;
    QSet<QUuid> m_pendingRequests;

public:
    static UserResolver& instance();

    UserResolver(const UserResolver&) = delete;
    UserResolver& operator=(const UserResolver&) = delete;
    UserResolver(UserResolver&&) = delete;
    UserResolver& operator=(UserResolver&&) = delete;

    /// Returns cached user data or starts an async server request and returns std::nullopt.
    std::optional<shared::PublicUserInfo> resolveUser(const QUuid& userId);

    void invalidateUser(const QUuid& userId);
    void clearCache();

signals:
    void userResolved(const QUuid& userId, const shared::PublicUserInfo& userInfo);

private:
    explicit UserResolver(QObject* parent = nullptr);

    void onPublicUserInfoReceived(const shared::PublicUserInfo& userInfo);
    void onConnectionStatusChanged();
};
