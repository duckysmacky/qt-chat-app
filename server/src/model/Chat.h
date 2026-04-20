#pragma once

#include <QDateTime>
#include <QString>
#include <QUuid>

namespace model {

/**
 * @class Chat
 * @brief Represents a chat entity in the system.
 */
class Chat
{
public:
    /**
     * @brief Default constructor. Creates a chat with default values.
     */
    Chat();

    /**
     * @brief Constructs a chat with specified parameters.
     * @param type The type of the chat (e.g., "private", "group").
     * @param createdBy The UUID of the user who created the chat.
     * @param title The title of the chat.
     */
    Chat(QString type,
         QUuid createdBy,
         QString title);

    /// @brief Returns the unique identifier of the chat.
    /// @return Constant reference to the chat UUID.
    const QUuid& id() const;

    /// @brief Returns the type of the chat.
    /// @return Constant reference to the chat type string.
    const QString& type() const;

    /// @brief Returns the UUID of the user who created the chat.
    /// @return Constant reference to the creator's UUID.
    const QUuid& createdBy() const;

    /// @brief Returns the title of the chat.
    /// @return Constant reference to the chat title.
    const QString& title() const;

    /// @brief Returns the creation date and time of the chat.
    /// @return Constant reference to the creation timestamp.
    const QDateTime& createdAt() const;

    /// @brief Sets the unique identifier of the chat.
    /// @param id The new chat UUID.
    void setId(const QUuid& id);

    /// @brief Sets the type of the chat.
    /// @param type The new chat type.
    void setType(const QString& type);

    /// @brief Sets the UUID of the user who created the chat.
    /// @param createdBy The new creator's UUID.
    void setCreatedBy(const QUuid& createdBy);

    /// @brief Sets the title of the chat.
    /// @param title The new chat title.
    void setTitle(const QString& title);

    /// @brief Sets the creation date and time of the chat.
    /// @param createdAt The new creation timestamp.
    void setCreatedAt(const QDateTime& createdAt);

private:
    QUuid m_id;           ///< Unique identifier of the chat
    QString m_type;       ///< Type of the chat (e.g., "private", "group")
    QUuid m_createdBy;    ///< UUID of the user who created the chat
    QString m_title;      ///< Title of the chat
    QDateTime m_createdAt; ///< Creation timestamp of the chat
};

} // namespace model