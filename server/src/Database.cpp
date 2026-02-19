#include "Database.h"

#include <QSqlError>
#include <QProcessEnvironment>
#include <QDebug>

Database& Database::instance()
{
	static Database instance;
	return instance;
}

bool Database::is_loaded() const
{
	return m_db.isOpen();
}

Database::Database()
	: m_db(QSqlDatabase::addDatabase("QPSQL"))
{
	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

	m_db.setHostName(env.value("DB_HOST"));
	m_db.setDatabaseName(env.value("DB_NAME"));
	m_db.setUserName(env.value("DB_USER"));
	m_db.setPassword(env.value("DB_PASSWORD"));
	
	if (m_db.open())
		qDebug() << "Database connected successfully";
	else
		qDebug() << "Database connection failed: " << m_db.lastError().text();
}

Database::~Database()
{
	m_db.close();
}
