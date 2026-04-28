#pragma once

#include <QString>
#include <QUuid>

namespace model {

/**
 * @class User
 * @brief Represents a user entity in the system.
 */
class User
{
public:
    /**
     * @brief Default constructor. Creates a User with default values.
     */
    User();

    /**
     * @brief Constructs a User with specified parameters.
     * @param username The username of the user.
     * @param name The display name of the user.
     * @param passwordHash The hashed password of the user.
     * @param email The email address of the user.
     */
    User(QString username,
         QString name,
         QString passwordHash,
         QString email);

    /// @brief Returns the unique identifier of the user.
    /// @return Constant reference to the user UUID.
    const QUuid& id() const;

    /// @brief Returns the username of the user.
    /// @return Constant reference to the username string.
    const QString& username() const;

    /// @brief Returns the display name of the user.
    /// @return Constant reference to the name string.
    const QString& name() const;

    /// @brief Returns the hashed password of the user.
    /// @return Constant reference to the password hash string.
    const QString& passwordHash() const;

    /// @brief Returns the email address of the user.
    /// @return Constant reference to the email string.
    const QString& email() const;

    /// @brief Sets the unique identifier of the user.
    /// @param id The new user UUID.
    void setId(const QUuid& id);

    /// @brief Sets the username of the user.
    /// @param username The new username.
    void setUsername(const QString& username);

    /// @brief Sets the display name of the user.
    /// @param name The new display name.
    void setName(const QString& name);

    /// @brief Sets the hashed password of the user.
    /// @param passwordHash The new password hash.
    void setPasswordHash(const QString& passwordHash);

    /// @brief Sets the email address of the user.
    /// @param email The new email address.
    void setEmail(const QString& email);

private:
    QUuid m_id;               ///< Unique identifier of the user
    QString m_username;       ///< Username of the user
    QString m_name;           ///< Display name of the user
    QString m_passwordHash;   ///< Hashed password of the user
    QString m_email;          ///< Email address of the user
};

} // namespace model