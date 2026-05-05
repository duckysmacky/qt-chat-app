#pragma once

#include <QDateTime>
#include <QString>
#include <QUuid>

namespace model {

enum class ChatType
{
    Direct,
    Group
};

QString chatTypeToString(ChatType type);
ChatType chatTypeFromString(const QString& type);

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
     * @param type The type of the chat.
     * @param createdBy The UUID of the user who created the chat.
     */
    Chat(ChatType type, QUuid createdBy);

    /// @brief Returns the unique identifier of the chat.
    /// @return Constant reference to the chat UUID.
    const QUuid& id() const;

    /// @brief Returns the type of the chat.
    ChatType type() const;

    /// @brief Returns the UUID of the user who created the chat.
    /// @return Constant reference to the creator's UUID.
    const QUuid& createdBy() const;

    /// @brief Returns the creation date and time of the chat.
    /// @return Constant reference to the creation timestamp.
    const QDateTime& createdAt() const;

    /// @brief Sets the unique identifier of the chat.
    /// @param id The new chat UUID.
    void setId(const QUuid& id);

    /// @brief Sets the type of the chat.
    /// @param type The new chat type.
    void setType(ChatType type);

    /// @brief Sets the UUID of the user who created the chat.
    /// @param createdBy The new creator's UUID.
    void setCreatedBy(const QUuid& createdBy);

    /// @brief Sets the creation date and time of the chat.
    /// @param createdAt The new creation timestamp.
    void setCreatedAt(const QDateTime& createdAt);

private:
    QUuid m_id;                         ///< Unique identifier of the chat
    ChatType m_type = ChatType::Direct; ///< Type of the chat
    QUuid m_createdBy;                  ///< UUID of the user who created the chat
    QDateTime m_createdAt;              ///< Creation timestamp of the chat
};

} // namespace model