#pragma once

#include <QSqlDatabase>
#include <QString>

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

	Database(const Database& other) = delete;
    Database& operator =(const Database& other) = delete;
	Database(Database&& other) = delete;
	Database& operator =(Database&& other) = delete;
	
private:
	Database();
	~Database();
};
