#include "Database.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QProcessEnvironment>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>

/**
 * @brief Returns the singleton database instance
 */
Database& Database::instance()
{
	static Database instance;
	return instance;
}

/**
 * @brief Establishes a PostgreSQL connection using environment variables
 *
 * Expected variables:
 * - DB_HOST
 * - DB_NAME
 * - DB_USER
 * - DB_PASSWORD
 *
 * @return true on successful connection
 */
bool Database::connect()
{
	if (m_initialized)
		return m_db.isOpen();

	const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

	m_db = QSqlDatabase::addDatabase("QPSQL", m_connectionName);

	m_db.setHostName(env.value("DB_HOST"));
	m_db.setDatabaseName(env.value("DB_NAME"));
	m_db.setUserName(env.value("DB_USER"));
	m_db.setPassword(env.value("DB_PASSWORD"));

	m_initialized = true;

	if (m_db.open()) {
		qInfo() << "Database connected successfully";
		return true;
	}

	qCritical() << "Database connection failed:" << m_db.lastError().text();
	return false;
}

/**
 * @brief Checks whether the database is connected
 */
bool Database::isConnected() const
{
	return m_db.isOpen();
}

/// @brief Construcnts the Database singleton
Database::Database()
	: m_connectionName("chat_app_server")
{
}

/// @brief Destroys the Database connection
Database::~Database()
{
	if (!m_initialized)
		return;

	m_db.close();

	m_db = QSqlDatabase();
	QSqlDatabase::removeDatabase(m_connectionName);
}

/** 
 * @brief Runs SQL migrations from migrations directory
 * and tracks versions into 'schema_migrations' table
 * @note requires an active database connection
 */
bool Database::init(){
	if (!m_db.isOpen()){
		return false;
	}

	QSqlQuery createTableQuery(m_db);
	if (!createTableQuery.exec("CREATE TABLE IF NOT EXISTS schema_migrations (version INTEGER PRIMARY KEY)")) {
		qCritical() << "Failed to create schema_migrations table:" << createTableQuery.lastError().text();
		return false;
	}

	int currentVersion = 0;
	QSqlQuery versionQuery(m_db);
	if (versionQuery.exec("SELECT MAX(version) FROM schema_migrations")) {
		if (versionQuery.next()) {
			currentVersion = versionQuery.value(0).toInt();
		}
	} else {
		qCritical() << "Failed to get current migration version:" << versionQuery.lastError().text();
		return false;
	}

	QDir dir("migrations");
	QStringList files = dir.entryList({"*.sql"}, QDir::Files, QDir::Name);
	int totalMigrations = files.size();

	for (int i = currentVersion; i < totalMigrations; ++i) {
		QString fileName = files[i];
		int version = i + 1;

		QString path = dir.filePath(fileName);
		QFile file(path);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			qCritical() << "Failed to open migration file:" << path;
			return false;
		}

		QString sql = file.readAll();
		file.close();

		if (!m_db.transaction()) {
			qCritical() << "Failed to start transaction for migration" << version;
			return false;
		}

		QSqlQuery query(m_db);
		if (!query.exec(sql)) {
			qCritical() << "Failed to execute migration" << version << ":" << query.lastError().text();
			m_db.rollback();
			return false;
		}

		QSqlQuery insertQuery(m_db);
		insertQuery.prepare("INSERT INTO schema_migrations (version) VALUES (?)");
		insertQuery.addBindValue(version);
		if (!insertQuery.exec()) {
			qCritical() << "Failed to record migration" << version << ":" << insertQuery.lastError().text();
			m_db.rollback();
			return false;
		}

		if (!m_db.commit()) {
			qCritical() << "Failed to commit migration" << version;
			return false;
		}
		currentVersion = version;
	}

	qInfo() << "All migrations executed successfully";
	return true;
}

/**
 * @brief Inserts a new user record into the database.
 * @param user The model::User object containing the data to insert.
 * @return true if the user was created successfully, false otherwise.
 */
bool Database::createUser(const model::User& user)
{
    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO users (id, username, name, password_hash, email) "
        "VALUES (:id, :username, :name, :password_hash, :email)"
        );

    query.bindValue(":id", user.id().toString(QUuid::WithoutBraces));
    query.bindValue(":username", user.username());
    query.bindValue(":name", user.name());
    query.bindValue(":password_hash", user.passwordHash());
    query.bindValue(":email", user.email());

    if (!query.exec()) {
        qCritical() << "Failed to create user:" << query.lastError().text();
        return false;
    }

    return true;
}

/**
 * @brief Deletes a user record from the database by its UUID.
 * @param id The QUuid of the user to delete.
 * @return true if at least one row was affected (user existed), false otherwise.
 */
bool Database::deleteUser(const QUuid& id)
{
    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM users WHERE id = :id");
    query.bindValue(":id", id.toString(QUuid::WithoutBraces));

    if (!query.exec()) {
        qCritical() << "Failed to delete user:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

/**
 * @brief Retrieves a user from the database by its UUID.
 * @param id The QUuid of the user to retrieve.
 * @return An std::optional containing the model::User if found, std::nullopt otherwise.
 */
std::optional<model::User> Database::getUserById(const QUuid& id) const
{
    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return std::nullopt;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, username, name, password_hash, email "
        "FROM users "
        "WHERE id = :id "
        "LIMIT 1"
        );

    query.bindValue(":id", id.toString(QUuid::WithoutBraces));

    if (!query.exec()) {
        qCritical() << "Failed to get user by id:" << query.lastError().text();
        return std::nullopt;
    }

    if (!query.next()) {
        return std::nullopt;
    }

    model::User user;
    user.setId(QUuid(query.value(0).toString()));
    user.setUsername(query.value(1).toString());
    user.setName(query.value(2).toString());
    user.setPasswordHash(query.value(3).toString());
    user.setEmail(query.value(4).toString());

    return user;
}

/**
 * @brief Retrieves all user records from the database.
 * @return A QList containing all model::User objects found. Returns an empty list on error.
 */
QList<model::User> Database::getAllUsers() const
{
    QList<model::User> users;

    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return users;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, username, name, password_hash, email "
        "FROM users"
        );

    if (!query.exec()) {
        qCritical() << "Failed to get all users:" << query.lastError().text();
        return users;
    }

    while (query.next()) {
        model::User user;
        user.setId(QUuid(query.value(0).toString()));
        user.setUsername(query.value(1).toString());
        user.setName(query.value(2).toString());
        user.setPasswordHash(query.value(3).toString());
        user.setEmail(query.value(4).toString());
        users.append(user);
    }

    return users;
}

/**
 * @brief Inserts a new content record into the database.
 * @param content The model::Content object containing the data to insert.
 * @return true if the content was created successfully, false otherwise.
 */
bool Database::createContent(const model::Content& content)
{
    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO contents (id, content, file, file_size) "
        "VALUES (:id, :content, :file, :file_size)"
        );

    query.bindValue(":id", content.id().toString(QUuid::WithoutBraces));
    query.bindValue(":content", content.content());
    query.bindValue(":file", content.file());
    query.bindValue(":file_size", content.fileSize());

    if (!query.exec()) {
        qCritical() << "Failed to create content:" << query.lastError().text();
        return false;
    }

    return true;
}

/**
 * @brief Deletes a content record from the database by its UUID.
 * @param id The QUuid of the content to delete.
 * @return true if at least one row was affected, false otherwise.
 */
bool Database::deleteContent(const QUuid& id)
{
    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM contents WHERE id = :id");
    query.bindValue(":id", id.toString(QUuid::WithoutBraces));

    if (!query.exec()) {
        qCritical() << "Failed to delete content:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

/**
 * @brief Retrieves a content record from the database by its UUID.
 * @param id The QUuid of the content to retrieve.
 * @return An std::optional containing the model::Content if found, std::nullopt otherwise.
 */
std::optional<model::Content> Database::getContentById(const QUuid& id) const
{
    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return std::nullopt;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, content, file, file_size "
        "FROM contents "
        "WHERE id = :id "
        "LIMIT 1"
        );

    query.bindValue(":id", id.toString(QUuid::WithoutBraces));

    if (!query.exec()) {
        qCritical() << "Failed to get content by id:" << query.lastError().text();
        return std::nullopt;
    }

    if (!query.next()) {
        return std::nullopt;
    }

    model::Content content;
    content.setId(QUuid(query.value(0).toString()));
    content.setContent(query.value(1).toString());
    content.setFile(query.value(2).toString());
    content.setFileSize(query.value(3).toDouble());

    return content;
}

/**
 * @brief Retrieves all content records from the database.
 * @return A QList containing all model::Content objects found. Returns an empty list on error.
 */
QList<model::Content> Database::getAllContents() const
{
    QList<model::Content> contents;

    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return contents;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, content, file, file_size "
        "FROM contents"
        );

    if (!query.exec()) {
        qCritical() << "Failed to get all contents:" << query.lastError().text();
        return contents;
    }

    while (query.next()) {
        model::Content content;
        content.setId(QUuid(query.value(0).toString()));
        content.setContent(query.value(1).toString());
        content.setFile(query.value(2).toString());
        content.setFileSize(query.value(3).toDouble());

        contents.append(content);
    }

    return contents;
}

/**
 * @brief Inserts a new chat record into the database.
 * @param chat The model::Chat object containing the data to insert.
 * @return true if the chat was created successfully, false otherwise.
 */
bool Database::createChat(const model::Chat& chat)
{
    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO chats (id, type, created_by, title, created_at) "
        "VALUES (:id, :type, :created_by, :title, :created_at)"
        );

    query.bindValue(":id", chat.id().toString(QUuid::WithoutBraces));
    query.bindValue(":type", chat.type());
    query.bindValue(":created_by", chat.createdBy().toString(QUuid::WithoutBraces));
    query.bindValue(":title", chat.title());
    query.bindValue(":created_at", chat.createdAt());

    if (!query.exec()) {
        qCritical() << "Failed to create chat:" << query.lastError().text();
        return false;
    }

    return true;
}

/**
 * @brief Deletes a chat record from the database by its UUID.
 * @param id The QUuid of the chat to delete.
 * @return true if at least one row was affected, false otherwise.
 */
bool Database::deleteChat(const QUuid& id)
{
    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM chats WHERE id = :id");
    query.bindValue(":id", id.toString(QUuid::WithoutBraces));

    if (!query.exec()) {
        qCritical() << "Failed to delete chat:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

/**
 * @brief Retrieves a chat record from the database by its UUID.
 * @param id The QUuid of the chat to retrieve.
 * @return An std::optional containing the model::Chat if found, std::nullopt otherwise.
 */
std::optional<model::Chat> Database::getChatById(const QUuid& id) const
{
    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return std::nullopt;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, type, created_by, title, created_at "
        "FROM chats "
        "WHERE id = :id "
        "LIMIT 1"
        );

    query.bindValue(":id", id.toString(QUuid::WithoutBraces));

    if (!query.exec()) {
        qCritical() << "Failed to get chat by id:" << query.lastError().text();
        return std::nullopt;
    }

    if (!query.next()) {
        return std::nullopt;
    }

    model::Chat chat;
    chat.setId(QUuid(query.value(0).toString()));
    chat.setType(query.value(1).toString());
    chat.setCreatedBy(QUuid(query.value(2).toString()));
    chat.setTitle(query.value(3).toString());
    chat.setCreatedAt(query.value(4).toDateTime());

    return chat;
}

/**
 * @brief Retrieves all chat records from the database.
 * @return A QList containing all model::Chat objects found. Returns an empty list on error.
 */
QList<model::Chat> Database::getAllChats() const
{
    QList<model::Chat> chats;

    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return chats;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, type, created_by, title, created_at "
        "FROM chats"
        );

    if (!query.exec()) {
        qCritical() << "Failed to get all chats:" << query.lastError().text();
        return chats;
    }

    while (query.next()) {
        model::Chat chat;
        chat.setId(QUuid(query.value(0).toString()));
        chat.setType(query.value(1).toString());
        chat.setCreatedBy(QUuid(query.value(2).toString()));
        chat.setTitle(query.value(3).toString());
        chat.setCreatedAt(query.value(4).toDateTime());

        chats.append(chat);
    }

    return chats;
}

/**
 * @brief Inserts a new chat member record into the database.
 * @param chatMember The model::ChatMember object containing the chat and user IDs.
 * @return true if the membership was created successfully, false otherwise.
 */
bool Database::createChatMember(const model::ChatMember& chatMember)
{
    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO chat_members (chat_id, user_id) "
        "VALUES (:chat_id, :user_id)"
        );

    query.bindValue(":chat_id", chatMember.chatId().toString(QUuid::WithoutBraces));
    query.bindValue(":user_id", chatMember.userId().toString(QUuid::WithoutBraces));

    if (!query.exec()) {
        qCritical() << "Failed to create chat member:" << query.lastError().text();
        return false;
    }

    return true;
}

/**
 * @brief Deletes a chat member record from the database.
 * @param chatId The QUuid of the chat.
 * @param userId The QUuid of the user.
 * @return true if at least one row was affected, false otherwise.
 */
bool Database::deleteChatMember(const QUuid& chatId, const QUuid& userId)
{
    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "DELETE FROM chat_members "
        "WHERE chat_id = :chat_id AND user_id = :user_id"
        );

    query.bindValue(":chat_id", chatId.toString(QUuid::WithoutBraces));
    query.bindValue(":user_id", userId.toString(QUuid::WithoutBraces));

    if (!query.exec()) {
        qCritical() << "Failed to delete chat member:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

/**
 * @brief Retrieves a specific chat member record from the database.
 * @param chatId The QUuid of the chat.
 * @param userId The QUuid of the user.
 * @return An std::optional containing the model::ChatMember if found, std::nullopt otherwise.
 */
std::optional<model::ChatMember> Database::getChatMember(const QUuid& chatId, const QUuid& userId) const
{
    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return std::nullopt;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT chat_id, user_id "
        "FROM chat_members "
        "WHERE chat_id = :chat_id AND user_id = :user_id "
        "LIMIT 1"
        );

    query.bindValue(":chat_id", chatId.toString(QUuid::WithoutBraces));
    query.bindValue(":user_id", userId.toString(QUuid::WithoutBraces));

    if (!query.exec()) {
        qCritical() << "Failed to get chat member:" << query.lastError().text();
        return std::nullopt;
    }

    if (!query.next()) {
        return std::nullopt;
    }

    model::ChatMember chatMember;
    chatMember.setChatId(QUuid(query.value(0).toString()));
    chatMember.setUserId(QUuid(query.value(1).toString()));

    return chatMember;
}

/**
 * @brief Retrieves all chat member records from the database.
 * @return A QList containing all model::ChatMember objects found. Returns an empty list on error.
 */
QList<model::ChatMember> Database::getAllChatMembers() const
{
    QList<model::ChatMember> chatMembers;

    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return chatMembers;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT chat_id, user_id "
        "FROM chat_members"
        );

    if (!query.exec()) {
        qCritical() << "Failed to get all chat members:" << query.lastError().text();
        return chatMembers;
    }

    while (query.next()) {
        model::ChatMember chatMember;
        chatMember.setChatId(QUuid(query.value(0).toString()));
        chatMember.setUserId(QUuid(query.value(1).toString()));

        chatMembers.append(chatMember);
    }

    return chatMembers;
}

/**
 * @brief Inserts a new message record into the database.
 * @param message The model::DbMessage object containing the data to insert.
 * @return true if the message was created successfully, false otherwise.
 */
bool Database::createMessage(const model::DbMessage& message)
{
    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO messages (id, to_id, from_id, content_id, chat_id, created_at, updated_at) "
        "VALUES (:id, :to_id, :from_id, :content_id, :chat_id, :created_at, :updated_at)"
        );

    query.bindValue(":id", message.id().toString(QUuid::WithoutBraces));
    query.bindValue(":to_id", message.toId().isNull()
                                  ? QVariant()
                                  : QVariant(message.toId().toString(QUuid::WithoutBraces)));
    query.bindValue(":from_id", message.fromId().isNull()
                                    ? QVariant()
                                    : QVariant(message.fromId().toString(QUuid::WithoutBraces)));
    query.bindValue(":content_id", message.contentId().toString(QUuid::WithoutBraces));
    query.bindValue(":chat_id", message.chatId().toString(QUuid::WithoutBraces));
    query.bindValue(":created_at", message.createdAt().isValid()
                                       ? QVariant(message.createdAt())
                                       : QVariant());
    query.bindValue(":updated_at", message.updatedAt().isValid()
                                       ? QVariant(message.updatedAt())
                                       : QVariant());

    if (!query.exec()) {
        qCritical() << "Failed to create message:" << query.lastError().text();
        return false;
    }

    return true;
}

/**
 * @brief Deletes a message record from the database by its UUID.
 * @param id The QUuid of the message to delete.
 * @return true if at least one row was affected, false otherwise.
 */
bool Database::deleteMessage(const QUuid& id)
{
    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM messages WHERE id = :id");
    query.bindValue(":id", id.toString(QUuid::WithoutBraces));

    if (!query.exec()) {
        qCritical() << "Failed to delete message:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

/**
 * @brief Retrieves a message record from the database by its UUID.
 * @param id The QUuid of the message to retrieve.
 * @return An std::optional containing the model::DbMessage if found, std::nullopt otherwise.
 */
std::optional<model::DbMessage> Database::getMessageById(const QUuid& id) const
{
    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return std::nullopt;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, to_id, from_id, content_id, chat_id, created_at, updated_at "
        "FROM messages "
        "WHERE id = :id "
        "LIMIT 1"
        );

    query.bindValue(":id", id.toString(QUuid::WithoutBraces));

    if (!query.exec()) {
        qCritical() << "Failed to get message by id:" << query.lastError().text();
        return std::nullopt;
    }

    if (!query.next()) {
        return std::nullopt;
    }

    model::DbMessage message;
    message.setId(QUuid(query.value(0).toString()));

    if (query.value(1).isNull())
        message.setToId(QUuid());
    else
        message.setToId(QUuid(query.value(1).toString()));

    if (query.value(2).isNull())
        message.setFromId(QUuid());
    else
        message.setFromId(QUuid(query.value(2).toString()));

    message.setContentId(QUuid(query.value(3).toString()));
    message.setChatId(QUuid(query.value(4).toString()));
    message.setCreatedAt(query.value(5).toDateTime());

    if (query.value(6).isNull())
        message.setUpdatedAt(QDateTime());
    else
        message.setUpdatedAt(query.value(6).toDateTime());

    return message;
}

/**
 * @brief Retrieves all message records from the database.
 * @return A QList containing all model::DbMessage objects found. Returns an empty list on error.
 */
QList<model::DbMessage> Database::getAllMessages() const
{
    QList<model::DbMessage> messages;

    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return messages;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, to_id, from_id, content_id, chat_id, created_at, updated_at "
        "FROM messages"
        );

    if (!query.exec()) {
        qCritical() << "Failed to get all messages:" << query.lastError().text();
        return messages;
    }

    while (query.next()) {
        model::DbMessage message;
        message.setId(QUuid(query.value(0).toString()));

        if (query.value(1).isNull())
            message.setToId(QUuid());
        else
            message.setToId(QUuid(query.value(1).toString()));

        if (query.value(2).isNull())
            message.setFromId(QUuid());
        else
            message.setFromId(QUuid(query.value(2).toString()));

        message.setContentId(QUuid(query.value(3).toString()));
        message.setChatId(QUuid(query.value(4).toString()));
        message.setCreatedAt(query.value(5).toDateTime());

        if (query.value(6).isNull())
            message.setUpdatedAt(QDateTime());
        else
            message.setUpdatedAt(query.value(6).toDateTime());

        messages.append(message);
    }

    return messages;
}

/**
 * @brief Converts a QStringList of usernames to a compact JSON array string.
 * @param usernames The list of usernames to convert.
 * @return A QString containing the JSON representation of the array.
 */
QString usernamesToJson(const QStringList& usernames)
{
    QJsonArray array;
    for (const auto& username : usernames) {
        array.append(username);
    }

    return QString::fromUtf8(
        QJsonDocument(array).toJson(QJsonDocument::Compact)
        );
}

/**
 * @brief Parses a JSON array string into a QStringList of usernames.
 * @param json The JSON string to parse.
 * @return A QStringList containing the extracted usernames. Returns an empty list if parsing fails.
 */
QStringList jsonToUsernames(const QString& json)
{
    QStringList usernames;

    if (json.isEmpty()) {
        return usernames;
    }

    const QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
    if (!document.isArray()) {
        return usernames;
    }

    for (const auto& value : document.array()) {
        usernames.append(value.toString());
    }

    return usernames;
}

/**
 * @brief Inserts a new user stats record into the database.
 * @param userStats The model::UserStats object containing the data to insert.
 * @return true if the user stats were created successfully, false otherwise.
 */
bool Database::createUserStats(const model::UserStats& userStats)
{
    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO user_stats (user_id, messages_sent, chats_joined, ex_usernames) "
        "VALUES (:user_id, :messages_sent, :chats_joined, CAST(:ex_usernames AS jsonb))"
        );

    query.bindValue(":user_id", userStats.userId().toString(QUuid::WithoutBraces));
    query.bindValue(":messages_sent", userStats.messagesSent());
    query.bindValue(":chats_joined", userStats.chatsJoined());
    query.bindValue(":ex_usernames", usernamesToJson(userStats.exUsernames()));

    if (!query.exec()) {
        qCritical() << "Failed to create user stats:" << query.lastError().text();
        return false;
    }

    return true;
}

/**
 * @brief Deletes a user stats record from the database by user ID.
 * @param userId The QUuid of the user whose stats should be deleted.
 * @return true if at least one row was affected, false otherwise.
 */
bool Database::deleteUserStats(const QUuid& userId)
{
    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM user_stats WHERE user_id = :user_id");
    query.bindValue(":user_id", userId.toString(QUuid::WithoutBraces));

    if (!query.exec()) {
        qCritical() << "Failed to delete user stats:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

/**
 * @brief Retrieves a user stats record from the database by user ID.
 * @param userId The QUuid of the user whose stats to retrieve.
 * @return An std::optional containing the model::UserStats if found, std::nullopt otherwise.
 */
std::optional<model::UserStats> Database::getUserStatsById(const QUuid& userId) const
{
    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return std::nullopt;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT user_id, messages_sent, chats_joined, ex_usernames::text "
        "FROM user_stats "
        "WHERE user_id = :user_id "
        "LIMIT 1"
        );

    query.bindValue(":user_id", userId.toString(QUuid::WithoutBraces));

    if (!query.exec()) {
        qCritical() << "Failed to get user stats by id:" << query.lastError().text();
        return std::nullopt;
    }

    if (!query.next()) {
        return std::nullopt;
    }

    model::UserStats userStats;
    userStats.setUserId(QUuid(query.value(0).toString()));
    userStats.setMessagesSent(query.value(1).toInt());
    userStats.setChatsJoined(query.value(2).toInt());

    if (query.value(3).isNull())
        userStats.setExUsernames({});
    else
        userStats.setExUsernames(jsonToUsernames(query.value(3).toString()));

    return userStats;
}

/**
 * @brief Retrieves all user stats records from the database.
 * @return A QList containing all model::UserStats objects found. Returns an empty list on error.
 */
QList<model::UserStats> Database::getAllUserStats() const
{
    QList<model::UserStats> userStatsList;

    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return userStatsList;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT user_id, messages_sent, chats_joined, ex_usernames::text "
        "FROM user_stats"
        );

    if (!query.exec()) {
        qCritical() << "Failed to get all user stats:" << query.lastError().text();
        return userStatsList;
    }

    while (query.next()) {
        model::UserStats userStats;
        userStats.setUserId(QUuid(query.value(0).toString()));
        userStats.setMessagesSent(query.value(1).toInt());
        userStats.setChatsJoined(query.value(2).toInt());

        if (query.value(3).isNull())
            userStats.setExUsernames({});
        else
            userStats.setExUsernames(jsonToUsernames(query.value(3).toString()));

        userStatsList.append(userStats);
    }

    return userStatsList;
}