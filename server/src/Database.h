#pragma once

#include <QSqlDatabase>
#include <QString>
#include <QUuid>
#include <optional>
#include "model.cpp"



class Database
{
private:
	QSqlDatabase m_db;
	QString m_connectionName;
	bool m_initialized = false;

public:
	static Database& instance();

	bool connect();
	bool isConnected() const;

	bool init();

    bool createUser(const model::User& user);
    bool deleteUser(const QUuid& id);
    std::optional<model::User> getUserById(const QUuid& id) const;
    QList<model::User> getAllUsers() const;

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
