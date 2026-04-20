#pragma once

#include <QByteArray>
#include <QString>

#include <optional>

namespace shared {

/**
 * @brief Lightweight container for login credentials.
 *
 * Provides JSON serialization and deserialization utilities.
 */
class LoginInfo
{
public:
    /// Default constructor.
    LoginInfo();

    /**
     * @brief Constructs login info.
     * @param login User login.
     * @param passwordHash Hashed password.
     */
    LoginInfo(QString login, QString passwordHash);

    /// Returns login string.
    const QString& login() const { return m_login; }

    /// Returns password hash.
    const QString& passwordHash() const { return m_passwordHash; }

    /**
     * @brief Serializes object to JSON.
     * @return QByteArray with JSON data.
     */
    QByteArray serialize() const;

    /**
     * @brief Deserializes object from JSON.
     * @param bytes Input JSON data.
     * @return Parsed LoginInfo or std::nullopt on failure.
     */
    static std::optional<LoginInfo> deserialize(const QByteArray& bytes);

private:
    QString m_login;
    QString m_passwordHash;
};

/**
 * @brief Lightweight container for user registration data.
 *
 * Provides JSON serialization and deserialization utilities.
 */
class RegisterInfo
{
public:
    /// Default constructor.
    RegisterInfo();

    /**
     * @brief Constructs registration info.
     * @param username Username.
     * @param name Display name.
     * @param email Email address.
     * @param passwordHash Hashed password.
     */
    RegisterInfo(QString username,
                 QString name,
                 QString email,
                 QString passwordHash);

    /// Returns username.
    const QString& username() const { return m_username; }

    /// Returns name.
    const QString& name() const { return m_name; }

    /// Returns email.
    const QString& email() const { return m_email; }

    /// Returns password hash.
    const QString& passwordHash() const { return m_passwordHash; }

    /**
     * @brief Serializes object to JSON.
     * @return QByteArray with JSON data.
     */
    QByteArray serialize() const;

    /**
     * @brief Deserializes object from JSON.
     * @param bytes Input JSON data.
     * @return Parsed RegisterInfo or std::nullopt on failure.
     */
    static std::optional<RegisterInfo> deserialize(const QByteArray& bytes);

private:
    QString m_username;
    QString m_name;
    QString m_email;
    QString m_passwordHash;
};

}