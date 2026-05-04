#pragma once

#include <QSqlDatabase>
#include <QString>
#include <QUuid>
#include <optional>
#include "dto/AuthInfo.h"
#include "dto/ProfileInfo.h"
#include "dto/ProfileUpdateInfo.h"
#include "dto/PublicUserInfo.h"
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
	/// @brief Returns the singleton instance of the Database.
	/// @return Reference to the Database object.
	static Database& instance();

	/**
	 * @brief Establishes a database connection.
	 * @return true if the connection was successfully established, false otherwise.
	 */
	bool connect();

	/**
	 * @brief Checks if the database is currently connected.
	 * @return true if connected, false otherwise.
	 */
	bool isConnected() const;

	/**
	 * @brief Initializes the database (creates tables, indices, etc.).
	 * @return true on success, false on failure.
	 */
	bool init();

	/// @brief Creates a new user in the database.
	/// @param user The user object to create.
	/// @return true on success, false otherwise.
    bool createUser(const model::User& user);
    QList<model::Chat> searchChats(const QString& query) const;

    bool deleteUser(const QUuid& id);

	/// @brief Retrieves a user by their unique identifier.
	/// @param id The UUID of the user.
	/// @return An optional containing the user if found, std::nullopt otherwise.
    std::optional<model::User> getUserById(const QUuid& id) const;

	/// @brief Retrieves all user IDs that belong to a specific chat.
	/// @param chatId The UUID of the chat.
	/// @return A list of user UUIDs.
    QList<QUuid> getUserIdsByChatId(const QUuid& chatId) const;

	/// @brief Retrieves all users from the database.
	/// @return A list of all users.
    QList<model::User> getAllUsers() const;

	/// @brief Retrieves a user by their username.
	/// @param username The username to search for.
	/// @return An optional containing the user if found, std::nullopt otherwise.
    std::optional<model::User> getUserByUsername(const QString& username) const;
    std::optional<model::User> getUserByEmail(const QString& email) const;
    std::optional<shared::ProfileInfo> getProfileInfoByUserId(const QUuid& userId) const;
    std::optional<shared::PublicUserInfo> getPublicUserInfoByUserId(const QUuid& userId) const;
    std::optional<model::User> updateUserProfile(const QUuid& userId, const shared::ProfileUpdateInfo& updateInfo);
    std::optional<model::User> authenticateUser(const shared::LoginInfo& loginInfo) const;

	/// @brief Creates a new content entry in the database.
	/// @param content The content object to create.
	/// @return true on success, false otherwise.
    bool createContent(const model::Content& content);

	/// @brief Deletes a content entry by its unique identifier.
	/// @param id The UUID of the content to delete.
	/// @return true on success, false otherwise.
    bool deleteContent(const QUuid& id);

	/// @brief Retrieves content by its unique identifier.
	/// @param id The UUID of the content.
	/// @return An optional containing the content if found, std::nullopt otherwise.
    std::optional<model::Content> getContentById(const QUuid& id) const;

	/// @brief Retrieves all content entries from the database.
	/// @return A list of all content objects.
    QList<model::Content> getAllContents() const;

	/// @brief Creates a new chat in the database.
	/// @param chat The chat object to create.
	/// @return true on success, false otherwise.
    bool createChat(const model::Chat& chat);

	/// @brief Deletes a chat by its unique identifier.
	/// @param id The UUID of the chat to delete.
	/// @return true on success, false otherwise.
    bool deleteChat(const QUuid& id);

	/// @brief Retrieves a chat by its unique identifier.
	/// @param id The UUID of the chat.
	/// @return An optional containing the chat if found, std::nullopt otherwise.
    std::optional<model::Chat> getChatById(const QUuid& id) const;

    /// @brief Retrieves chats that the specified user belongs to.
    /// @param userId The UUID of the user.
    /// @return A list of chat objects for the user.
    QList<model::Chat> getChatsByUserId(const QUuid& userId) const;

	/// @brief Adds a member to a chat.
	/// @param chatMember The chat member relationship to create.
	/// @return true on success, false otherwise.
    bool createChatMember(const model::ChatMember& chatMember);

	/// @brief Removes a member from a chat.
	/// @param chatId The UUID of the chat.
	/// @param userId The UUID of the user to remove.
	/// @return true on success, false otherwise.
    bool deleteChatMember(const QUuid& chatId, const QUuid& userId);

	/// @brief Retrieves a specific chat member relationship.
	/// @param chatId The UUID of the chat.
	/// @param userId The UUID of the user.
	/// @return An optional containing the chat member if found, std::nullopt otherwise.
    std::optional<model::ChatMember> getChatMember(const QUuid& chatId, const QUuid& userId) const;

	/// @brief Retrieves all chat member relationships from the database.
	/// @return A list of all chat member objects.
    QList<model::ChatMember> getAllChatMembers() const;

	/// @brief Creates a new message in the database.
	/// @param message The message object to create.
	/// @return true on success, false otherwise.
    bool createMessage(const model::DbMessage& message);

	/// @brief Deletes a message by its unique identifier.
	/// @param id The UUID of the message to delete.
	/// @return true on success, false otherwise.
    bool deleteMessage(const QUuid& id);

	/// @brief Retrieves a message by its unique identifier.
	/// @param id The UUID of the message.
	/// @return An optional containing the message if found, std::nullopt otherwise.
    std::optional<model::DbMessage> getMessageById(const QUuid& id) const;

	/// @brief Retrieves all messages from the database.
	/// @return A list of all message objects.
    QList<model::DbMessage> getAllMessages() const;

	/// @brief Creates user statistics in the database.
	/// @param userStats The user statistics object to create.
	/// @return true on success, false otherwise.
    bool createUserStats(const model::UserStats& userStats);

	/// @brief Deletes user statistics by user ID.
	/// @param userId The UUID of the user whose statistics should be deleted.
	/// @return true on success, false otherwise.
    bool deleteUserStats(const QUuid& userId);

	/// @brief Retrieves user statistics by user ID.
	/// @param userId The UUID of the user.
	/// @return An optional containing the user statistics if found, std::nullopt otherwise.
    std::optional<model::UserStats> getUserStatsById(const QUuid& userId) const;

	/// @brief Retrieves all user statistics from the database.
	/// @return A list of all user statistics objects.
    QList<model::UserStats> getAllUserStats() const;

	/// @brief Copy constructor is deleted (singleton pattern).
	Database(const Database& other) = delete;

	/// @brief Copy assignment operator is deleted (singleton pattern).
    Database& operator =(const Database& other) = delete;

	/// @brief Move constructor is deleted (singleton pattern).
	Database(Database&& other) = delete;

	/// @brief Move assignment operator is deleted (singleton pattern).
	Database& operator =(Database&& other) = delete;
	
private:
	/// @brief Private constructor (singleton pattern).
	Database();

	/// @brief Private destructor (singleton pattern).
	~Database();
};
