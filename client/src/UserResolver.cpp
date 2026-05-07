#include "UserResolver.h"

#include <QDebug>

#include <utility>

#include "Client.h"
#include "RequestManager.h"
#include "util.h"

UserResolver& UserResolver::instance()
{
    static UserResolver instance;
    return instance;
}

UserResolver::UserResolver(QObject* parent)
    : QObject(parent)
{
    const RequestManager& requestManager = RequestManager::instance();
    const Client& client = Client::instance();

    connect(&requestManager, &RequestManager::publicUserInfoReceived, this, &UserResolver::onPublicUserInfoReceived);
    connect(&requestManager, &RequestManager::operationResultReceived, this, &UserResolver::onOperationResultReceived);
    connect(&client, &Client::connectionStatusChanged, this, &UserResolver::onConnectionStatusChanged);
}

std::optional<shared::PublicUserInfo> UserResolver::resolveUser(const QUuid& userId)
{
    if (userId.isNull())
        return std::nullopt;

    if (m_cache.contains(userId))
        return m_cache.value(userId);

    if (m_pendingRequests.contains(userId))
        return std::nullopt;

    if (!Client::instance().connected())
        return std::nullopt;

    m_pendingRequests.insert(userId);
    RequestManager::instance().getUserInfo(userId);

    return std::nullopt;
}

std::optional<shared::PublicUserInfo> UserResolver::resolveUser(QString username)
{
    username = shared::util::normalizeUsername(std::move(username));
    if (username.isEmpty())
        return std::nullopt;

    if (m_usernameCache.contains(username))
        return m_cache.value(m_usernameCache.value(username));

    if (m_pendingUsernameRequests.contains(username))
        return std::nullopt;

    if (!Client::instance().connected())
        return std::nullopt;

    m_pendingUsernameRequests.insert(username);
    RequestManager::instance().getUserInfo(username);

    return std::nullopt;
}

void UserResolver::invalidateUser(const QUuid& userId)
{
    if (m_cache.contains(userId))
        m_usernameCache.remove(m_cache.value(userId).username());

    m_cache.remove(userId);
    m_pendingRequests.remove(userId);
}

void UserResolver::invalidateUser(QString username)
{
    username = shared::util::normalizeUsername(std::move(username));
    if (username.isEmpty())
        return;

    if (m_usernameCache.contains(username))
    {
        const QUuid userId = m_usernameCache.take(username);
        m_cache.remove(userId);
        m_pendingRequests.remove(userId);
    }

    m_pendingUsernameRequests.remove(username);
}

void UserResolver::clearCache()
{
    m_cache.clear();
    m_usernameCache.clear();
    m_pendingRequests.clear();
    m_pendingUsernameRequests.clear();
}

void UserResolver::onPublicUserInfoReceived(const shared::PublicUserInfo& userInfo)
{
    const QUuid& userId = userInfo.userId();
    if (userId.isNull()) {
        qWarning() << "Ignoring public user info with invalid user id";
        return;
    }

    m_pendingRequests.remove(userId);
    m_pendingUsernameRequests.remove(userInfo.username());
    m_cache.insert(userId, userInfo);
    m_usernameCache.insert(userInfo.username(), userId);
    emit userResolved(userId, userInfo);
    emit userResolvedByUsername(userInfo.username(), userInfo);
}

void UserResolver::onOperationResultReceived(const shared::OperationResult& result)
{
    if (result.type() != shared::OperationResultType::ERROR)
        return;

    m_pendingRequests.clear();
    m_pendingUsernameRequests.clear();
}

void UserResolver::onConnectionStatusChanged()
{
    if (Client::instance().connected())
        return;

    m_pendingRequests.clear();
    m_pendingUsernameRequests.clear();
}
