#pragma once

#include <QSqlDatabase>

class Database
{
private:
	QSqlDatabase m_db;

public:
	static Database& instance();

	bool is_loaded() const;

	Database(const Database& other) = delete;
    Database& operator =(const Database& other) = delete;
	
private:
	Database();
	~Database();
};

