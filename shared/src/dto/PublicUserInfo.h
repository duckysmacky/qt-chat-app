/**
 * @file PublicUserInfo.h
 * @brief Definition of the PublicUserInfo DTO class for publicly available user data.
 */

#pragma once

#include <QByteArray>
#include <QString>
#include <QUuid>

#include <optional>

namespace shared {

/**
 * @class PublicUserInfo
 * @brief Data transfer object containing publicly accessible user information.
 * 
 * The PublicUserInfo class encapsulates user data that is safe to share publicly,
 * such as user ID, username, and display name. This DTO is used when transmitting
 * user information that does not contain sensitive or private data (e.g., email,
 * password hash, etc.).
 */
class PublicUserInfo
{
public:
    /**
     * @brief Default constructor.
     * 
     * Creates an empty PublicUserInfo object with uninitialized fields.
     */
    PublicUserInfo();
    
    /**
     * @brief Constructs a PublicUserInfo with complete user public data.
     * @param userId The unique identifier of the user.
     * @param username The user's unique username.
     * @param displayName The user's display name (may contain spaces and special characters).
     */
    PublicUserInfo(QUuid userId, QString username, QString displayName);

    /**
     * @brief Returns the unique identifier of the user.
     * @return Constant reference to the user's UUID.
     */
    const QUuid& userId() const { return m_userId; }
    
    /**
     * @brief Returns the username of the user.
     * @return Constant reference to the username string.
     */
    const QString& username() const { return m_username; }
    
    /**
     * @brief Returns the display name of the user.
     * @return Constant reference to the display name string.
     */
    const QString& displayName() const { return m_displayName; }

    /**
     * @brief Serializes the PublicUserInfo object into a byte array.
     * @return QByteArray containing the serialized data.
     * 
     * Converts the PublicUserInfo object into a byte array suitable for network transmission
     * or persistent storage. The serialized format includes all public user fields.
     */
    QByteArray serialize() const;
    
    /**
     * @brief Deserializes a byte array into a PublicUserInfo object.
     * @param bytes The byte array to deserialize.
     * @return std::optional<PublicUserInfo> containing the deserialized object if successful,
     *         or std::nullopt if deserialization fails.
     * 
     * Reconstructs a PublicUserInfo object from its serialized byte representation.
     * Returns an empty optional if the byte array is invalid or corrupted.
     */
    static std::optional<PublicUserInfo> deserialize(const QByteArray& bytes);

private:
    QUuid m_userId;        ///< Unique identifier of the user.
    QString m_username;    ///< Unique username of the user (used for login/mention).
    QString m_displayName; ///< Display name shown in UI (may contain spaces and special characters).
};

} // namespace shared