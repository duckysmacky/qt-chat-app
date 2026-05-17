/**
 * @file ProfileInfo.h
 * @brief Defines the ProfileInfo class for user profile data.
 */

#pragma once

#include <QByteArray>
#include <QString>
#include <QUuid>

#include <optional>

namespace shared {

/**
 * @class ProfileInfo
 * @brief Represents a user's complete profile information.
 * 
 * The ProfileInfo class encapsulates all profile data for an authenticated user,
 * including user ID, username, display name, and email address. Provides
 * serialization and deserialization functionality for network transmission.
 */
class ProfileInfo
{
public:
    /**
     * @brief Default constructor.
     * 
     * Constructs an empty ProfileInfo object with default values.
     */
    ProfileInfo();
    
    /**
     * @brief Constructs a ProfileInfo with complete user data.
     * @param userId UUID of the user.
     * @param username The user's unique username.
     * @param displayName The user's display name (may be same as username).
     * @param email The user's email address.
     */
    ProfileInfo(QUuid userId, QString username, QString displayName, QString email);

    /**
     * @brief Gets the user's UUID.
     * @return Constant reference to the user ID.
     */
    const QUuid& userId() const { return m_userId; }
    
    /**
     * @brief Gets the user's username.
     * @return Constant reference to the username.
     */
    const QString& username() const { return m_username; }
    
    /**
     * @brief Gets the user's display name.
     * @return Constant reference to the display name.
     */
    const QString& displayName() const { return m_displayName; }
    
    /**
     * @brief Gets the user's email address.
     * @return Constant reference to the email.
     */
    const QString& email() const { return m_email; }

    /**
     * @brief Serializes the ProfileInfo object into a byte array.
     * @return QByteArray containing the serialized profile data.
     */
    QByteArray serialize() const;
    
    /**
     * @brief Deserializes a byte array into a ProfileInfo object.
     * @param bytes The byte array to deserialize.
     * @return Optional containing the deserialized ProfileInfo if successful,
     *         or std::nullopt if deserialization fails.
     */
    static std::optional<ProfileInfo> deserialize(const QByteArray& bytes);

private:
    QUuid m_userId;         ///< Unique identifier for the user
    QString m_username;     ///< User's unique username for login
    QString m_displayName;  ///< User's display name shown to others
    QString m_email;        ///< User's email address
};

}