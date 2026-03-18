#include "Database.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QProcessEnvironment>
#include <QDebug>
#include <QDir>
#include <QFile>

Database& Database::instance()
{
	static Database instance;
	return instance;
}

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

bool Database::isConnected() const
{
	return m_db.isOpen();
}

Database::Database()
	: m_connectionName("chat_app_server")
{
}

Database::~Database()
{
	if (!m_initialized)
		return;

	m_db.close();

	m_db = QSqlDatabase();
	QSqlDatabase::removeDatabase(m_connectionName);
}

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

bool Database::createUser(const QString& username, const QString& name, const QString& passwordHash, const QString& email)
{
    if (!m_db.isOpen()) {
        qWarning() << "Database is not connected";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO users (username, name, password_hash, email) "
        "VALUES (:username, :name, :password_hash, :email)"
        );

    query.bindValue(":username", username);
    query.bindValue(":name", name);
    query.bindValue(":password_hash", passwordHash);
    query.bindValue(":email", email);

    if (!query.exec()) {
        qCritical() << "Failed to create user:" << query.lastError().text();
        return false;
    }

    return true;
}

std::optional<User> Database::getUserById(const QUuid& id) const
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

    User user;
    user.id = QUuid(query.value(0).toString());
    user.username = query.value(1).toString();
    user.name = query.value(2).toString();
    user.passwordHash = query.value(3).toString();
    user.email = query.value(4).toString();

    return user;
}
