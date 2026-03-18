#pragma once

#include <QSqlDatabase>
#include <QString>
#include <QUuid>
#include <optional>


struct User
{
    QUuid id;
    QString username;
    QString name;
    QString passwordHash;
    QString email;
};

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

    bool createUser(const QString& username, const QString& name, const QString& passwordHash, const QString& email);

    std::optional<User> getUserById(const QUuid& id) const;

	Database(const Database& other) = delete;
    Database& operator =(const Database& other) = delete;
	Database(Database&& other) = delete;
	Database& operator =(Database&& other) = delete;
	
private:
	Database();
	~Database();
};
