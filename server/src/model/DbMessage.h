#pragma once

#include <QDateTime>
#include <QUuid>

namespace model {

/**
 * @class DbMessage
 * @brief Represents a message stored in the database.
 */
class DbMessage
{
public:
    /**
     * @brief Default constructor. Creates a DbMessage with default values.
     */
    DbMessage();

    /**
     * @brief Constructs a DbMessage with specified parameters.
     * @param toId The UUID of the message recipient.
     * @param fromId The UUID of the message sender.
     * @param contentId The UUID of the content associated with the message.
     * @param chatId The UUID of the chat this message belongs to.
     */
    DbMessage(QUuid toId,
              QUuid fromId,
              QUuid contentId,
              QUuid chatId);

    /// @brief Returns the unique identifier of the message.
    /// @return Constant reference to the message UUID.
    const QUuid& id() const;

    /// @brief Returns the UUID of the message recipient.
    /// @return Constant reference to the recipient's UUID.
    const QUuid& toId() const;

    /// @brief Returns the UUID of the message sender.
    /// @return Constant reference to the sender's UUID.
    const QUuid& fromId() const;

    /// @brief Returns the UUID of the content associated with the message.
    /// @return Constant reference to the content UUID.
    const QUuid& contentId() const;

    /// @brief Returns the UUID of the chat this message belongs to.
    /// @return Constant reference to the chat UUID.
    const QUuid& chatId() const;

    /// @brief Returns the creation timestamp of the message.
    /// @return Constant reference to the creation timestamp.
    const QDateTime& createdAt() const;

    /// @brief Returns the last update timestamp of the message.
    /// @return Constant reference to the update timestamp.
    const QDateTime& updatedAt() const;

    /// @brief Sets the unique identifier of the message.
    /// @param id The new message UUID.
    void setId(const QUuid& id);

    /// @brief Sets the UUID of the message recipient.
    /// @param toId The new recipient's UUID.
    void setToId(const QUuid& toId);

    /// @brief Sets the UUID of the message sender.
    /// @param fromId The new sender's UUID.
    void setFromId(const QUuid& fromId);

    /// @brief Sets the UUID of the content associated with the message.
    /// @param contentId The new content UUID.
    void setContentId(const QUuid& contentId);

    /// @brief Sets the UUID of the chat this message belongs to.
    /// @param chatId The new chat UUID.
    void setChatId(const QUuid& chatId);

    /// @brief Sets the creation timestamp of the message.
    /// @param createdAt The new creation timestamp.
    void setCreatedAt(const QDateTime& createdAt);

    /// @brief Sets the last update timestamp of the message.
    /// @param updatedAt The new update timestamp.
    void setUpdatedAt(const QDateTime& updatedAt);

private:
    QUuid m_id;         ///< Unique identifier of the message
    QUuid m_toId;       ///< UUID of the message recipient
    QUuid m_fromId;     ///< UUID of the message sender
    QUuid m_contentId;  ///< UUID of the associated content
    QUuid m_chatId;     ///< UUID of the chat this message belongs to
    QDateTime m_createdAt; ///< Creation timestamp
    QDateTime m_updatedAt; ///< Last update timestamp
};

} // namespace model