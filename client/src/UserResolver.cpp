#include "UserResolver.h"

#include <QDebug>

#include "Client.h"
#include "RequestManager.h"

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
    RequestManager::instance().getPublicUserInfo(userId);

    return std::nullopt;
}

void UserResolver::invalidateUser(const QUuid& userId)
{
    m_cache.remove(userId);
    m_pendingRequests.remove(userId);
}

void UserResolver::clearCache()
{
    m_cache.clear();
    m_pendingRequests.clear();
}

void UserResolver::onPublicUserInfoReceived(const shared::PublicUserInfo& userInfo)
{
    const QUuid& userId = userInfo.userId();
    if (userId.isNull()) {
        qWarning() << "Ignoring public user info with invalid user id";
        return;
    }

    m_pendingRequests.remove(userId);
    m_cache.insert(userId, userInfo);
    emit userResolved(userId, userInfo);
}

void UserResolver::onConnectionStatusChanged()
{
    if (Client::instance().connected())
        return;

    m_pendingRequests.clear();
}
