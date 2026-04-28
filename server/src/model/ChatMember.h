#pragma once

#include <QUuid>

namespace model {

/**
 * @class ChatMember
 * @brief Represents a many-to-many relationship between users and chats.
 */
class ChatMember
{
public:
    /**
     * @brief Default constructor. Creates a ChatMember with default values.
     */
    ChatMember();

    /**
     * @brief Constructs a ChatMember with specified chat and user IDs.
     * @param chatId The UUID of the chat.
     * @param userId The UUID of the user.
     */
    ChatMember(QUuid chatId,
               QUuid userId);

    /// @brief Returns the UUID of the chat.
    /// @return Constant reference to the chat ID.
    const QUuid& chatId() const;

    /// @brief Returns the UUID of the user.
    /// @return Constant reference to the user ID.
    const QUuid& userId() const;

    /// @brief Sets the UUID of the chat.
    /// @param chatId The new chat ID.
    void setChatId(const QUuid& chatId);

    /// @brief Sets the UUID of the user.
    /// @param userId The new user ID.
    void setUserId(const QUuid& userId);

private:
    QUuid m_chatId; ///< Unique identifier of the chat
    QUuid m_userId; ///< Unique identifier of the user
};

} // namespace model