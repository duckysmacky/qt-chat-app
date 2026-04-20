#include "AuthInfo.h"

#include <QJsonDocument>
#include <QJsonObject>

#include <utility>

namespace shared {

/**
 * @class LoginInfo
 * @brief Represents user login credentials and provides JSON serialization/deserialization utilities.
 *
 * This class stores login-related information (login and password hash) and
 * provides methods to serialize the data into JSON format and restore it from JSON.
 */
LoginInfo::LoginInfo() = default;

/**
 * @brief Constructs a LoginInfo object.
 * @param login User login identifier.
 * @param passwordHash Hashed password string.
 */
LoginInfo::LoginInfo(QString login, QString passwordHash)
    : m_login(std::move(login)),
    m_passwordHash(std::move(passwordHash))
{
}

/**
 * @brief Serializes the LoginInfo object into a compact JSON byte array.
 * @return QByteArray containing JSON representation of the object.
 *
 * The resulting JSON object contains the following fields:
 * - login
 * - passwordHash
 */
QByteArray LoginInfo::serialize() const
{
    QJsonObject obj;
    obj["login"] = m_login;
    obj["passwordHash"] = m_passwordHash;

    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

/**
 * @brief Deserializes a LoginInfo object from a JSON byte array.
 * @param bytes Input JSON data.
 * @return std::optional containing LoginInfo if deserialization succeeds, otherwise std::nullopt.
 *
 * The function validates that the JSON object contains required fields:
 * - login
 * - passwordHash
 */
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

/**
 * @class RegisterInfo
 * @brief Represents user registration information and provides JSON serialization/deserialization utilities.
 *
 * This class stores full registration data including username, name, email,
 * and password hash. It supports conversion to and from JSON format.
 */
RegisterInfo::RegisterInfo() = default;

/**
 * @brief Constructs a RegisterInfo object.
 * @param username Unique username.
 * @param name Display name of the user.
 * @param email User email address.
 * @param passwordHash Hashed password string.
 */
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

/**
 * @brief Serializes the RegisterInfo object into a compact JSON byte array.
 * @return QByteArray containing JSON representation of the object.
 *
 * The resulting JSON object contains the following fields:
 * - username
 * - name
 * - email
 * - passwordHash
 */
QByteArray RegisterInfo::serialize() const
{
    QJsonObject obj;
    obj["username"] = m_username;
    obj["name"] = m_name;
    obj["email"] = m_email;
    obj["passwordHash"] = m_passwordHash;

    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

/**
 * @brief Deserializes a RegisterInfo object from a JSON byte array.
 * @param bytes Input JSON data.
 * @return std::optional containing RegisterInfo if deserialization succeeds, otherwise std::nullopt.
 *
 * The function validates that the JSON object contains all required fields:
 * - username
 * - name
 * - email
 * - passwordHash
 */
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