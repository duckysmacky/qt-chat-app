/**
 * @file UserResolver.h
 * @brief Definition of the UserResolver singleton class for user data resolution and caching.
 */

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
 * 
 * The UserResolver class provides a centralized mechanism for looking up public user information.
 * It maintains an internal cache of resolved users to minimize server requests and supports
 * asynchronous resolution when data is not available locally.
 */
class UserResolver : public QObject
{
    Q_OBJECT

private:
    QHash<QUuid, shared::PublicUserInfo> m_cache;           ///< Cache mapping user IDs to their public information.
    QHash<QString, QUuid> m_usernameCache;                  ///< Cache mapping usernames to user IDs for quick lookup.
    QSet<QUuid> m_pendingRequests;                          ///< Set of user IDs currently being requested from the server.
    QSet<QString> m_pendingUsernameRequests;                ///< Set of usernames currently being requested from the server.

public:
    /**
     * @brief Returns the singleton instance of the UserResolver.
     * @return Reference to the unique UserResolver instance.
     */
    static UserResolver& instance();

    // Deleted copy/move constructors and assignment operators to enforce singleton pattern.
    UserResolver(const UserResolver&) = delete;
    UserResolver& operator=(const UserResolver&) = delete;
    UserResolver(UserResolver&&) = delete;
    UserResolver& operator=(UserResolver&&) = delete;

    /**
     * @brief Resolves user information by user ID.
     * @param userId The unique identifier of the user to resolve.
     * @return std::optional<shared::PublicUserInfo> containing the user info if cached,
     *         or std::nullopt if the data needs to be fetched asynchronously.
     * 
     * If the user data is already cached, it is returned immediately.
     * Otherwise, an asynchronous server request is initiated and std::nullopt is returned.
     * When the data becomes available, the userResolved() signal will be emitted.
     */
    std::optional<shared::PublicUserInfo> resolveUser(const QUuid& userId);
    
    /**
     * @brief Resolves user information by username.
     * @param username The username of the user to resolve.
     * @return std::optional<shared::PublicUserInfo> containing the user info if cached,
     *         or std::nullopt if the data needs to be fetched asynchronously.
     * 
     * If the user data is already cached (via username lookup), it is returned immediately.
     * Otherwise, an asynchronous server request is initiated and std::nullopt is returned.
     * When the data becomes available, the userResolvedByUsername() signal will be emitted.
     */
    std::optional<shared::PublicUserInfo> resolveUser(QString username);

    /**
     * @brief Invalidates the cached user data for the specified user ID.
     * @param userId The unique identifier of the user to invalidate.
     * 
     * Removes the user from both ID and username caches. Future resolveUser() calls
     * for this user will trigger a fresh server request.
     */
    void invalidateUser(const QUuid& userId);
    
    /**
     * @brief Invalidates the cached user data for the specified username.
     * @param username The username of the user to invalidate.
     * 
     * Removes the user from both ID and username caches. Future resolveUser() calls
     * for this username will trigger a fresh server request.
     */
    void invalidateUser(QString username);
    
    /**
     * @brief Clears the entire user cache.
     * 
     * Removes all cached user data. All pending requests remain active,
     * but future resolveUser() calls will treat users as uncached.
     */
    void clearCache();

signals:
    /**
     * @brief Emitted when user information has been successfully resolved by ID.
     * @param userId The unique identifier of the resolved user.
     * @param userInfo The public information of the resolved user.
     */
    void userResolved(const QUuid& userId, const shared::PublicUserInfo& userInfo);
    
    /**
     * @brief Emitted when user information has been successfully resolved by username.
     * @param username The username of the resolved user.
     * @param userInfo The public information of the resolved user.
     */
    void userResolvedByUsername(const QString& username, const shared::PublicUserInfo& userInfo);

private:
    /**
     * @brief Private constructor for singleton pattern.
     * @param parent Optional QObject parent pointer.
     */
    explicit UserResolver(QObject* parent = nullptr);

    /**
     * @brief Handles incoming public user information from the server.
     * @param userInfo The received user information to cache and forward.
     * 
     * Updates internal caches and emits appropriate resolution signals.
     */
    void onPublicUserInfoReceived(const shared::PublicUserInfo& userInfo);
    
    /**
     * @brief Handles operation results from the server.
     * @param result The operation result containing success/failure information.
     * 
     * Processes pending requests that may have failed and cleans up request tracking.
     */
    void onOperationResultReceived(const shared::OperationResult& result);
    
    /**
     * @brief Handles changes in the server connection status.
     * 
     * May trigger retry logic for pending requests or clear pending request queues
     * depending on the connection state.
     */
    void onConnectionStatusChanged();
};