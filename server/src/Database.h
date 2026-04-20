#pragma once

#include <QSqlDatabase>
#include <QString>
#include <QUuid>
#include <optional>
#include "model.h"

/**
 * @class Database
 * @brief Singleton responsible for managing database connection and operations.
 *
 * Provides CRUD operations for all domain models and handles connection lifecycle.
 */
class Database
{
private:
    QSqlDatabase m_db;              ///< Database connection object
    QString m_connectionName;       ///< Qt connection name identifier
    bool m_initialized = false;     ///< Indicates whether the database is initialized

public:
    /**
     * @brief Returns singleton instance of Database.
     * @return Reference to Database instance.
     */
    static Database& instance();

    /**
     * @brief Establishes a connection to the database.
     * @return True if connection was successfully established, otherwise false.
     */
    bool connect();

    /**
     * @brief Checks whether the database is currently connected.
     * @return True if connected, otherwise false.
     */
    bool isConnected() const;

    /**
     * @brief Initializes database schema and required structures.
     * @return True on successful initialization.
     */
    bool init();

    // ========================= USER OPERATIONS =========================

    bool createUser(const model::User& user);
    bool deleteUser(const QUuid& id);
    std::optional<model::User> getUserById(const QUuid& id) const;
    QList<model::User> getAllUsers() const;

    // ========================= CONTENT OPERATIONS =========================

    bool createContent(const model::Content& content);
    bool deleteContent(const QUuid& id);
    std::optional<model::Content> getContentById(const QUuid& id) const;
    QList<model::Content> getAllContents() const;

    // ========================= CHAT OPERATIONS =========================

    bool createChat(const model::Chat& chat);
    bool deleteChat(const QUuid& id);
    std::optional<model::Chat> getChatById(const QUuid& id) const;
    QList<model::Chat> getAllChats() const;

    // ========================= CHAT MEMBER OPERATIONS =========================

    bool createChatMember(const model::ChatMember& chatMember);
    bool deleteChatMember(const QUuid& chatId, const QUuid& userId);
    std::optional<model::ChatMember> getChatMember(const QUuid& chatId, const QUuid& userId) const;
    QList<model::ChatMember> getAllChatMembers() const;

    // ========================= MESSAGE OPERATIONS =========================

    bool createMessage(const model::DbMessage& message);
    bool deleteMessage(const QUuid& id);
    std::optional<model::DbMessage> getMessageById(const QUuid& id) const;
    QList<model::DbMessage> getAllMessages() const;

    // ========================= USER STATS OPERATIONS =========================

    bool createUserStats(const model::UserStats& userStats);
    bool deleteUserStats(const QUuid& userId);
    std::optional<model::UserStats> getUserStatsById(const QUuid& userId) const;
    QList<model::UserStats> getAllUserStats() const;

    // ========================= RULE OF FIVE =========================

    Database(const Database& other) = delete;
    Database& operator =(const Database& other) = delete;
    Database(Database&& other) = delete;
    Database& operator =(Database&& other) = delete;

private:
    /**
     * @brief Private constructor (singleton pattern).
     */
    Database();

    /**
     * @brief Destructor.
     */
    ~Database();
};