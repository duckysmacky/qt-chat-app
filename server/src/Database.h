#pragma once

#include <QSqlDatabase>
#include <QString>
#include <QUuid>
#include <optional>
#include "AuthInfo.h"
#include "ProfileInfo.h"
#include "ProfileUpdateInfo.h"
#include "PublicUserInfo.h"
#include "model/Chat.h"
#include "model/ChatMember.h"
#include "model/Content.h"
#include "model/DbMessage.h"
#include "model/User.h"
#include "model/UserStats.h"


/**
 * @class Database
 * @brief Singleton for database connection managing
 */
class Database
{
private:
	QSqlDatabase m_db; ///< Connection object
	QString m_connectionName; ///< Connection name (using Qt for connection management)
	bool m_initialized = false; ///< Shows if db is initialized (true/false)

public:
	/// @brief Returns the singleton
	/// @return Link to Database object
	static Database& instance();

	/**
	 * @brief Establishes a database connection.
	 * @returns true/false
	 */
	bool connect();

	/**
	 * @brief Checks if the database is connected.
	 */
	bool isConnected() const;

	/**
	 * @brief Initializes the database
	 * @return true on success
	 */
	bool init();

    bool createUser(const model::User& user);
    bool deleteUser(const QUuid& id);
    std::optional<model::User> getUserById(const QUuid& id) const;
    QList<QUuid> getUserIdsByChatId(const QUuid& chatId) const;

    QList<model::User> getAllUsers() const;
    std::optional<model::User> getUserByUsername(const QString& username) const;
    std::optional<model::User> getUserByEmail(const QString& email) const;
    std::optional<shared::ProfileInfo> getProfileInfoByUserId(const QUuid& userId) const;
    std::optional<shared::PublicUserInfo> getPublicUserInfoByUserId(const QUuid& userId) const;
    std::optional<model::User> updateUserProfile(const QUuid& userId, const shared::ProfileUpdateInfo& updateInfo);
    std::optional<model::User> authenticateUser(const shared::LoginInfo& loginInfo) const;

    bool createContent(const model::Content& content);
    bool deleteContent(const QUuid& id);
    std::optional<model::Content> getContentById(const QUuid& id) const;
    QList<model::Content> getAllContents() const;

    bool createChat(const model::Chat& chat);
    bool deleteChat(const QUuid& id);
    std::optional<model::Chat> getChatById(const QUuid& id) const;
    QList<model::Chat> getAllChats() const;

    bool createChatMember(const model::ChatMember& chatMember);
    bool deleteChatMember(const QUuid& chatId, const QUuid& userId);
    std::optional<model::ChatMember> getChatMember(const QUuid& chatId, const QUuid& userId) const;
    QList<model::ChatMember> getAllChatMembers() const;

    bool createMessage(const model::DbMessage& message);
    bool deleteMessage(const QUuid& id);
    std::optional<model::DbMessage> getMessageById(const QUuid& id) const;
    QList<model::DbMessage> getAllMessages() const;

    bool createUserStats(const model::UserStats& userStats);
    bool deleteUserStats(const QUuid& userId);
    std::optional<model::UserStats> getUserStatsById(const QUuid& userId) const;
    QList<model::UserStats> getAllUserStats() const;

	Database(const Database& other) = delete;
    Database& operator =(const Database& other) = delete;
	Database(Database&& other) = delete;
	Database& operator =(Database&& other) = delete;
	
private:
	Database();
	~Database();
};
