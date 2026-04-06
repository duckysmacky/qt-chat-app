#include "AuthInfo.h"

#include <QJsonDocument>
#include <QJsonObject>

#include <utility>

namespace shared {

LoginInfo::LoginInfo() = default;

LoginInfo::LoginInfo(QString login, QString passwordHash)
    : m_login(std::move(login)),
    m_passwordHash(std::move(passwordHash))
{
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

RegisterInfo::RegisterInfo(QString username,
                           QString name,
                           QString email,
                           QString passwordHash)
    : m_username(std::move(username)),
    m_name(std::move(name)),
    m_email(std::move(email)),
    m_passwordHash(std::move(passwordHash))
{
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
