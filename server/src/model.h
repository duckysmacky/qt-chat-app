#pragma once

#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QUuid>

namespace model {

/**
 * @brief User entity of the system.
 *
 * Stores core user information: unique identifier, username,
 * display name, password hash, and email.
 */
class User
{
public:
    /**
     * @brief Default constructor.
     */
    User();

    /**
     * @brief Constructs a user with initial data.
     * @param username User login name.
     * @param name Display name.
     * @param password Plain-text password (will be hashed internally).
     * @param email User email address.
     */
    User(const QString& username,
         const QString& name,
         const QString& password,
         const QString& email);

    /// @return Unique user identifier.
    const QUuid& id() const;

    /// @return Username (login).
    const QString& username() const;

    /// @return Display name.
    const QString& name() const;

    /// @return Password hash.
    const QString& passwordHash() const;

    /// @return User email.
    const QString& email() const;

    /// Sets user ID.
    void setId(const QUuid& id);

    /// Sets username.
    void setUsername(const QString& username);

    /// Sets display name.
    void setName(const QString& name);

    /// Sets password (will be hashed internally).
    void setPassword(const QString& password);

    /// Sets already hashed password.
    void setPasswordHash(const QString& passwordHash);

    /// Sets user email.
    void setEmail(const QString& email);

private:
    /**
     * @brief Hashes a password using internal hashing algorithm.
     * @param password Plain-text password.
     * @return Hashed password string.
     */
    static QString hashPassword(const QString& password);

private:
    QUuid m_id;              ///< User ID
    QString m_username;      ///< Username
    QString m_name;          ///< Display name
    QString m_passwordHash;  ///< Password hash
    QString m_email;         ///< Email address
};

/**
 * @brief Content model (message or file).
 *
 * Used to store textual content and optional file metadata.
 */
class Content
{
public:
    Content();

    /**
     * @brief Constructs content object.
     * @param content Text content.
     * @param file File name.
     * @param fileSize File size in bytes.
     */
    Content(const QString& content,
            const QString& file,
            double fileSize);

    /// @return Content ID.
    const QUuid& id() const;

    /// @return Text content.
    const QString& content() const;

    /// @return File name.
    const QString& file() const;

    /// @return File size in bytes.
    double fileSize() const;

    void setId(const QUuid& id);
    void setContent(const QString& content);
    void setFile(const QString& file);
    void setFileSize(double fileSize);

private:
    QUuid m_id;          ///< Content ID
    QString m_content;   ///< Text content
    QString m_file;      ///< File name
    double m_fileSize;   ///< File size
};

/**
 * @brief Chat entity.
 *
 * Represents chat metadata such as type, creator, title, and creation time.
 */
class Chat
{
public:
    Chat();

    /**
     * @brief Constructs chat instance.
     * @param type Chat type.
     * @param createdBy Creator user ID.
     * @param title Chat title.
     */
    Chat(const QString& type,
         const QUuid& createdBy,
         const QString& title);

    const QUuid& id() const;
    const QString& type() const;
    const QUuid& createdBy() const;
    const QString& title() const;
    const QDateTime& createdAt() const;

    void setId(const QUuid& id);
    void setType(const QString& type);
    void setCreatedBy(const QUuid& createdBy);
    void setTitle(const QString& title);
    void setCreatedAt(const QDateTime& createdAt);

private:
    QUuid m_id;            ///< Chat ID
    QString m_type;        ///< Chat type
    QUuid m_createdBy;     ///< Creator ID
    QString m_title;       ///< Chat title
    QDateTime m_createdAt; ///< Creation timestamp
};

/**
 * @brief Chat membership relation.
 *
 * Stores association between users and chats.
 */
class ChatMember
{
public:
    ChatMember();

    /**
     * @brief Constructs chat membership relation.
     * @param chatId Chat ID.
     * @param userId User ID.
     */
    ChatMember(const QUuid& chatId,
               const QUuid& userId);

    const QUuid& chatId() const;
    const QUuid& userId() const;

    void setChatId(const QUuid& chatId);
    void setUserId(const QUuid& userId);

private:
    QUuid m_chatId; ///< Chat ID
    QUuid m_userId; ///< User ID
};

/**
 * @brief Database message entity.
 *
 * Contains metadata about a message including sender, receiver,
 * content reference, and chat association.
 */
class DbMessage
{
public:
    DbMessage();

    /**
     * @brief Constructs message entity.
     * @param toId Receiver ID.
     * @param fromId Sender ID.
     * @param contentId Content ID.
     * @param chatId Chat ID.
     */
    DbMessage(const QUuid& toId,
              const QUuid& fromId,
              const QUuid& contentId,
              const QUuid& chatId);

    const QUuid& id() const;
    const QUuid& toId() const;
    const QUuid& fromId() const;
    const QUuid& contentId() const;
    const QUuid& chatId() const;
    const QDateTime& createdAt() const;
    const QDateTime& updatedAt() const;

    void setId(const QUuid& id);
    void setToId(const QUuid& toId);
    void setFromId(const QUuid& fromId);
    void setContentId(const QUuid& contentId);
    void setChatId(const QUuid& chatId);
    void setCreatedAt(const QDateTime& createdAt);
    void setUpdatedAt(const QDateTime& updatedAt);

private:
    QUuid m_id;            ///< Message ID
    QUuid m_toId;          ///< Receiver ID
    QUuid m_fromId;        ///< Sender ID
    QUuid m_contentId;     ///< Content ID
    QUuid m_chatId;        ///< Chat ID
    QDateTime m_createdAt; ///< Creation timestamp
    QDateTime m_updatedAt; ///< Last update timestamp
};

/**
 * @brief User statistics model.
 *
 * Stores aggregated user activity data such as message count and chats joined.
 */
class UserStats
{
public:
    UserStats();

    /**
     * @brief Constructs user statistics.
     * @param userId User ID.
     * @param messagesSent Number of sent messages.
     * @param chatsJoined Number of joined chats.
     * @param exUsernames List of previous usernames.
     */
    UserStats(const QUuid& userId,
              int messagesSent = 0,
              int chatsJoined = 0,
              const QStringList& exUsernames = {});

    const QUuid& userId() const;
    int messagesSent() const;
    int chatsJoined() const;
    const QStringList& exUsernames() const;

    void setUserId(const QUuid& userId);
    void setMessagesSent(int messagesSent);
    void setChatsJoined(int chatsJoined);
    void setExUsernames(const QStringList& exUsernames);

private:
    QUuid m_userId;            ///< User ID
    int m_messagesSent;        ///< Sent messages count
    int m_chatsJoined;         ///< Joined chats count
    QStringList m_exUsernames; ///< Previous usernames history
};

} // namespace model