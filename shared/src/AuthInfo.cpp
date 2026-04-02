#include "AuthInfo.h"

#include <QJsonDocument>
#include <QJsonObject>

namespace shared {

LoginInfo::LoginInfo() = default;

LoginInfo::LoginInfo(const QString& login, const QString& passwordHash)
    : m_login(login),
    m_passwordHash(passwordHash)
{
}

const QString& LoginInfo::login() const
{
    return m_login;
}

const QString& LoginInfo::passwordHash() const
{
    return m_passwordHash;
}

QByteArray LoginInfo::serialize() const
{
    QJsonObject obj;
    obj["login"] = m_login;
    obj["passwordHash"] = m_passwordHash;

    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

std::optional<LoginInfo> LoginInfo::deserialize(const QByteArray& bytes)
{
    const QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (!doc.isObject()) {
        return std::nullopt;
    }

    const QJsonObject obj = doc.object();

    if (!obj.contains("login") || !obj.contains("passwordHash")) {
        return std::nullopt;
    }

    return LoginInfo(
        obj["login"].toString(),
        obj["passwordHash"].toString()
        );
}

RegisterInfo::RegisterInfo() = default;

RegisterInfo::RegisterInfo(const QString& username,
                           const QString& name,
                           const QString& email,
                           const QString& passwordHash)
    : m_username(username),
    m_name(name),
    m_email(email),
    m_passwordHash(passwordHash)
{
}

const QString& RegisterInfo::username() const
{
    return m_username;
}

const QString& RegisterInfo::name() const
{
    return m_name;
}

const QString& RegisterInfo::email() const
{
    return m_email;
}

const QString& RegisterInfo::passwordHash() const
{
    return m_passwordHash;
}

QByteArray RegisterInfo::serialize() const
{
    QJsonObject obj;
    obj["username"] = m_username;
    obj["name"] = m_name;
    obj["email"] = m_email;
    obj["passwordHash"] = m_passwordHash;

    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

std::optional<RegisterInfo> RegisterInfo::deserialize(const QByteArray& bytes)
{
    const QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (!doc.isObject()) {
        return std::nullopt;
    }

    const QJsonObject obj = doc.object();

    if (!obj.contains("username") ||
        !obj.contains("name") ||
        !obj.contains("email") ||
        !obj.contains("passwordHash")) {
        return std::nullopt;
    }

    return RegisterInfo(
        obj["username"].toString(),
        obj["name"].toString(),
        obj["email"].toString(),
        obj["passwordHash"].toString()
        );
}

}
