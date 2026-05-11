/**
 * @file ProfileUpdateInfo.h
 * @brief Defines the ProfileUpdateInfo class for partial profile updates.
 */

#pragma once

#include <QByteArray>
#include <QString>

#include <optional>

namespace shared {

/**
 * @class ProfileUpdateInfo
 * @brief Represents partial profile update data for user modification.
 * 
 * The ProfileUpdateInfo class encapsulates optional fields that can be updated
 * on a user's profile. Only fields that are set (have a value) will be updated,
 * allowing partial updates without requiring all fields to be specified.
 */
class ProfileUpdateInfo
{
public:
    /**
     * @brief Default constructor.
     * 
     * Constructs an empty ProfileUpdateInfo object with no fields set.
     */
    ProfileUpdateInfo() = default;

    /**
     * @brief Gets the optional new username.
     * @return Constant optional reference to the username, or std::nullopt if not being updated.
     */
    const std::optional<QString>& username() const { return m_username; }
    
    /**
     * @brief Gets the optional new display name.
     * @return Constant optional reference to the display name, or std::nullopt if not being updated.
     */
    const std::optional<QString>& displayName() const { return m_displayName; }
    
    /**
     * @brief Gets the optional new email address.
     * @return Constant optional reference to the email, or std::nullopt if not being updated.
     */
    const std::optional<QString>& email() const { return m_email; }
    
    /**
     * @brief Gets the optional new password hash.
     * @return Constant optional reference to the password hash, or std::nullopt if not being updated.
     */
    const std::optional<QString>& passwordHash() const { return m_passwordHash; }

    /**
     * @brief Sets the username to be updated.
     * @param username The new username value.
     */
    void setUsername(QString username) { m_username = std::move(username); }
    
    /**
     * @brief Sets the display name to be updated.
     * @param displayName The new display name value.
     */
    void setDisplayName(QString displayName) { m_displayName = std::move(displayName); }
    
    /**
     * @brief Sets the email address to be updated.
     * @param email The new email value.
     */
    void setEmail(QString email) { m_email = std::move(email); }
    
    /**
     * @brief Sets the password hash to be updated.
     * @param passwordHash The new password hash value.
     */
    void setPasswordHash(QString passwordHash) { m_passwordHash = std::move(passwordHash); }

    /**
     * @brief Checks if any fields are set for update.
     * @return True if all optional fields are empty (no updates to perform),
     *         false if at least one field has a value.
     */
    bool isEmpty() const;

    /**
     * @brief Serializes the ProfileUpdateInfo object into a byte array.
     * @return QByteArray containing the serialized profile update data.
     */
    QByteArray serialize() const;
    
    /**
     * @brief Deserializes a byte array into a ProfileUpdateInfo object.
     * @param bytes The byte array to deserialize.
     * @return Optional containing the deserialized ProfileUpdateInfo if successful,
     *         or std::nullopt if deserialization fails.
     */
    static std::optional<ProfileUpdateInfo> deserialize(const QByteArray& bytes);

private:
    std::optional<QString> m_username;      ///< Optional new username for the profile
    std::optional<QString> m_displayName;   ///< Optional new display name for the profile
    std::optional<QString> m_email;         ///< Optional new email address for the profile
    std::optional<QString> m_passwordHash;  ///< Optional new password hash for the profile
};

}