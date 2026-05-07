#pragma once

#include <QHash>
#include <QObject>
#include <QSet>
#include <QString>
#include <QUuid>

#include <optional>

#include "OperationResult.h"
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
    QHash<QString, QUuid> m_usernameCache;
    QSet<QUuid> m_pendingRequests;
    QSet<QString> m_pendingUsernameRequests;

public:
    static UserResolver& instance();

    UserResolver(const UserResolver&) = delete;
    UserResolver& operator=(const UserResolver&) = delete;
    UserResolver(UserResolver&&) = delete;
    UserResolver& operator=(UserResolver&&) = delete;

    /// Returns cached user data or starts an async server request and returns std::nullopt.
    std::optional<shared::PublicUserInfo> resolveUser(const QUuid& userId);
    std::optional<shared::PublicUserInfo> resolveUser(QString username);

    void invalidateUser(const QUuid& userId);
    void invalidateUser(QString username);
    void clearCache();

signals:
    void userResolved(const QUuid& userId, const shared::PublicUserInfo& userInfo);
    void userResolvedByUsername(const QString& username, const shared::PublicUserInfo& userInfo);

private:
    explicit UserResolver(QObject* parent = nullptr);

    void onPublicUserInfoReceived(const shared::PublicUserInfo& userInfo);
    void onOperationResultReceived(const shared::OperationResult& result);
    void onConnectionStatusChanged();
};
