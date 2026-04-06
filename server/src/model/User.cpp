#include "User.h"

#include <utility>

namespace model {

User::User() = default;

User::User(QString username,
           QString name,
           QString passwordHash,
           QString email)
    : m_id(QUuid::createUuid()),
      m_username(std::move(username)),
      m_name(std::move(name)),
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

const QString& User::name() const
{
    return m_name;
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

void User::setName(const QString& name)
{
    m_name = name;
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
