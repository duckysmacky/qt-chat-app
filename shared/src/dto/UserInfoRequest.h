/**
 * @file UserInfoRequest.h
 * @brief Definition of the UserInfoRequest DTO class for user information queries.
 */

#pragma once

#include <QByteArray>
#include <QString>
#include <QUuid>

#include <optional>

namespace shared {

/**
 * @enum UserIdentifierType
 * @brief Specifies the type of identifier used in a user information request.
 * 
 * This enumeration distinguishes between different ways to identify a user
 * when requesting their information.
 */
enum class UserIdentifierType
{
    UUID,     ///< User is identified by their UUID (Universally Unique Identifier).
    USERNAME  ///< User is identified by their unique username.
};

/**
 * @class UserInfoRequest
 * @brief Data transfer object for requesting user information.
 * 
 * The UserInfoRequest class encapsulates a request to retrieve user information.
 * It supports two types of user identification: by UUID or by username.
 * This DTO is used when the client needs to request information about a specific user.
 */
class UserInfoRequest
{
public:
    /**
     * @brief Default constructor.
     * 
     * Creates an empty UserInfoRequest object with uninitialized fields.
     */
    UserInfoRequest();
    
    /**
     * @brief Constructs a UserInfoRequest using a user UUID.
     * @param userId The UUID of the user to request.
     * 
     * Creates a request that identifies the user by their unique UUID identifier.
     */
    explicit UserInfoRequest(QUuid userId);
    
    /**
     * @brief Constructs a UserInfoRequest using a username.
     * @param username The username of the user to request.
     * 
     * Creates a request that identifies the user by their unique username.
     */
    explicit UserInfoRequest(QString username);

    /**
     * @brief Returns the type of identifier used in this request.
     * @return UserIdentifierType indicating whether UUID or username is used.
     */
    UserIdentifierType identifierType() const { return m_identifierType; }
    
    /**
     * @brief Returns the user UUID (valid only when identifierType() is UUID).
     * @return Constant reference to the user's UUID.
     */
    const QUuid& userId() const { return m_userId; }
    
    /**
     * @brief Returns the username (valid only when identifierType() is USERNAME).
     * @return Constant reference to the username string.
     */
    const QString& username() const { return m_username; }

    /**
     * @brief Serializes the UserInfoRequest object into a byte array.
     * @return QByteArray containing the serialized data.
     * 
     * Converts the UserInfoRequest object into a byte array suitable for network transmission.
     * The serialized format includes the identifier type and the corresponding identifier value.
     */
    QByteArray serialize() const;
    
    /**
     * @brief Deserializes a byte array into a UserInfoRequest object.
     * @param bytes The byte array to deserialize.
     * @return std::optional<UserInfoRequest> containing the deserialized object if successful,
     *         or std::nullopt if deserialization fails.
     * 
     * Reconstructs a UserInfoRequest object from its serialized byte representation.
     * Returns an empty optional if the byte array is invalid or corrupted.
     */
    static std::optional<UserInfoRequest> deserialize(const QByteArray& bytes);

private:
    UserIdentifierType m_identifierType;  ///< Type of identifier used (UUID or USERNAME).
    QUuid m_userId;                       ///< User UUID (valid when m_identifierType is UUID).
    QString m_username;                   ///< Username (valid when m_identifierType is USERNAME).
};

} // namespace shared