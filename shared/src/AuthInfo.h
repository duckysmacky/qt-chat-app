#pragma once

#include <QByteArray>
#include <QString>

#include <optional>

namespace shared {

/**
 * @class LoginInfo
 * @brief Contains login credentials for user authentication.
 */
class LoginInfo
{
public:
    /**
     * @brief Default constructor. Creates an empty LoginInfo object.
     */
    LoginInfo();

    /**
     * @brief Constructs a LoginInfo with specified credentials.
     * @param login The username or email used for login.
     * @param passwordHash The hashed password of the user.
     */
    LoginInfo(QString login, QString passwordHash);

    /// @brief Returns the login string (username or email).
    /// @return Constant reference to the login string.
    const QString& login() const { return m_login; }

    /// @brief Returns the hashed password.
    /// @return Constant reference to the password hash string.
    const QString& passwordHash() const { return m_passwordHash; }

    /**
     * @brief Serializes the LoginInfo object into a byte array.
     * @return QByteArray containing the serialized data.
     */
    QByteArray serialize() const;

    /**
     * @brief Deserializes a byte array into a LoginInfo object.
     * @param bytes The byte array to deserialize.
     * @return An optional containing the deserialized LoginInfo if successful, std::nullopt otherwise.
     */
    static std::optional<LoginInfo> deserialize(const QByteArray& bytes);

private:
    QString m_login;         ///< Username or email used for login
    QString m_passwordHash;  ///< Hashed password of the user
};

/**
 * @class RegisterInfo
 * @brief Contains registration information for creating a new user account.
 */
class RegisterInfo
{
public:
    /**
     * @brief Default constructor. Creates an empty RegisterInfo object.
     */
    RegisterInfo();

    /**
     * @brief Constructs a RegisterInfo with specified user details.
     * @param username The desired username.
     * @param name The display name of the user.
     * @param email The email address of the user.
     * @param passwordHash The hashed password of the user.
     */
    RegisterInfo(QString username,
                 QString name,
                 QString email,
                 QString passwordHash);

    /// @brief Returns the username.
    /// @return Constant reference to the username string.
    const QString& username() const { return m_username; }

    /// @brief Returns the display name.
    /// @return Constant reference to the name string.
    const QString& name() const { return m_name; }

    /// @brief Returns the email address.
    /// @return Constant reference to the email string.
    const QString& email() const { return m_email; }

    /// @brief Returns the hashed password.
    /// @return Constant reference to the password hash string.
    const QString& passwordHash() const { return m_passwordHash; }

    /**
     * @brief Serializes the RegisterInfo object into a byte array.
     * @return QByteArray containing the serialized data.
     */
    QByteArray serialize() const;

    /**
     * @brief Deserializes a byte array into a RegisterInfo object.
     * @param bytes The byte array to deserialize.
     * @return An optional containing the deserialized RegisterInfo if successful, std::nullopt otherwise.
     */
    static std::optional<RegisterInfo> deserialize(const QByteArray& bytes);

private:
    QString m_username;      ///< Desired username
    QString m_name;          ///< Display name of the user
    QString m_email;         ///< Email address of the user
    QString m_passwordHash;  ///< Hashed password of the user
};

} // namespace shared