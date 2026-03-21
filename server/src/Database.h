#pragma once

#include <QSqlDatabase>
#include <QString>

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


	Database(const Database& other) = delete;
    Database& operator =(const Database& other) = delete;
	Database(Database&& other) = delete;
	Database& operator =(Database&& other) = delete;
	
private:
	Database();
	~Database();
};
