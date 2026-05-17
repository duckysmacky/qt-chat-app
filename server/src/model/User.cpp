#include "User.h"

#include <utility>

namespace model {

User::User() = default;

User::User(QString username,
           QString displayName,
           QString passwordHash,
           QString email)
    : m_id(QUuid::createUuid()),
      m_username(std::move(username)),
      m_displayName(std::move(displayName)),
      m_passwordHash(std::move(passwordHash)),
      m_email(std::move(email))
{
}

const QUuid& User::id() const
{
    return m_id;
}

const QString& User::username() const
{
    return m_username;
}

const QString& User::displayName() const
{
    return m_displayName;
}

const QString& User::passwordHash() const
{
    return m_passwordHash;
}

const QString& User::email() const
{
    return m_email;
}

void User::setId(const QUuid& id)
{
    m_id = id;
}

void User::setUsername(const QString& username)
{
    m_username = username;
}

void User::setDisplayName(const QString& displayName)
{
    m_displayName = displayName;
}

void User::setPasswordHash(const QString& passwordHash)
{
    m_passwordHash = passwordHash;
}

void User::setEmail(const QString& email)
{
    m_email = email;
}

}
