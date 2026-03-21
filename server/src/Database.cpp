#include "Database.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QProcessEnvironment>
#include <QDebug>
#include <QDir>
#include <QFile>

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
