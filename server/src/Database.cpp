#include "Database.h"

#include <QSqlError>
#include <QProcessEnvironment>
#include <QDebug>

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
