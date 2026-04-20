#pragma once

#include <QStringList>
#include <QUuid>

namespace model {

/**
 * @class UserStats
 * @brief Represents statistical data and history for a user.
 */
class UserStats
{
public:
    /**
     * @brief Default constructor. Creates a UserStats object with default values.
     */
    UserStats();

    /**
     * @brief Constructs a UserStats object with specified parameters.
     * @param userId The UUID of the user.
     * @param messagesSent Number of messages sent by the user (default 0).
     * @param chatsJoined Number of chats the user has joined (default 0).
     * @param exUsernames List of previously used usernames (default empty).
     */
    UserStats(QUuid userId,
              int messagesSent = 0,
              int chatsJoined = 0,
              QStringList exUsernames = {});

    /// @brief Returns the UUID of the user.
    /// @return Constant reference to the user UUID.
    const QUuid& userId() const;

    /// @brief Returns the number of messages sent by the user.
    /// @return Messages sent count.
    int messagesSent() const;

    /// @brief Returns the number of chats the user has joined.
    /// @return Chats joined count.
    int chatsJoined() const;

    /// @brief Returns the list of previously used usernames.
    /// @return Constant reference to the list of ex-usernames.
    const QStringList& exUsernames() const;

    /// @brief Sets the UUID of the user.
    /// @param userId The new user UUID.
    void setUserId(const QUuid& userId);

    /// @brief Sets the number of messages sent by the user.
    /// @param messagesSent The new messages sent count.
    void setMessagesSent(int messagesSent);

    /// @brief Sets the number of chats the user has joined.
    /// @param chatsJoined The new chats joined count.
    void setChatsJoined(int chatsJoined);

    /// @brief Sets the list of previously used usernames.
    /// @param exUsernames The new list of ex-usernames.
    void setExUsernames(const QStringList& exUsernames);

private:
    QUuid m_userId;           ///< Unique identifier of the user
    int m_messagesSent = 0;   ///< Number of messages sent by the user
    int m_chatsJoined = 0;    ///< Number of chats the user has joined
    QStringList m_exUsernames; ///< List of previously used usernames
};

} // namespace model