#pragma once

#include <QObject>
#include <QByteArray>
#include <QHash>
#include <QUuid>

#include "Chat.h"
#include "dto/ChatsInfo.h"
#include "dto/ChatKeyInfo.h"

/**
 * @brief Singleton manager for chat objects.
 *
 * The ChatManager class maintains a collection of Chat instances,
 * provides access to the current chat list, and handles chat selection.
 * It integrates with the UI via Qt properties and invokable methods.
 */
class ChatManager : public QObject
{
    Q_OBJECT
    /// Exposes the list of all chats to QML.
    Q_PROPERTY(QList<Chat*> chats READ chats NOTIFY chatsChanged)
    /// Exposes the currently selected chat to QML.
    Q_PROPERTY(Chat* selectedChat READ selectedChat NOTIFY selectedChatChanged)

private:
    QHash<QUuid, Chat*> m_chatStorage; ///< Hash map for fast lookup by chat ID.
    QHash<QUuid, QByteArray> m_pendingChatMasterKeys;
    QList<Chat*> m_chatList;           ///< List of all chats, used for QML access.
    Chat* m_selectedChat;              ///< Currently selected chat, or nullptr.

public:
    /**
     * @brief Returns the singleton instance of ChatManager.
     * @return Reference to the unique ChatManager instance.
     */
    static ChatManager& instance();

    // Deleted copy/move constructors and assignment operators to enforce singleton pattern.
    ChatManager(const ChatManager&) = delete;
    ChatManager& operator=(const ChatManager&) = delete;
    ChatManager(ChatManager&&) = delete;
    ChatManager& operator=(ChatManager&&) = delete;

    /**
     * @brief Selects a chat by its ID.
     * @param chatId The unique identifier of the chat to select.
     *
     * If the chat exists, it becomes the new selectedChat() and the
     * selectedChatChanged() signal is emitted. Does nothing if the ID is invalid.
     */
    Q_INVOKABLE void selectChat(const QUuid& chatId);

    /**
     * @brief Clears the current chat selection.
     *
     * Sets selectedChat() to nullptr and emits selectedChatChanged().
     */
    Q_INVOKABLE void unselectChat();

    /**
     * @brief Adds a new chat to the manager.
     * @param chat Pointer to the Chat object to add.
     *
     * The ChatManager takes ownership of the chat object.
     * If a chat with the same ID already exists, the new one replaces it.
     * The chatsChanged() signal is emitted after the list is updated.
     */
    void addChat(Chat* chat);

    void setChatMasterKey(const QUuid& chatId, QByteArray masterKey);

    /**
     * @brief Removes a chat by its ID.
     * @param chatId The unique identifier of the chat to remove.
     *
     * If the removed chat was the selected one, the selection is cleared.
     * The chat object is deleted. Emits chatsChanged() and potentially
     * selectedChatChanged().
     */
    void removeChat(const QUuid& chatId);

    /**
     * @brief Returns the const reference to the list of all chats.
     * @return QList<Chat*>& containing all managed chats.
     */
    const QList<Chat*>& chats() const { return m_chatList; }

    /**
     * @brief Returns the currently selected chat.
     * @return Pointer to the selected Chat, or nullptr if none is selected.
     */
    Chat* selectedChat() const { return m_selectedChat; }

signals:
    /**
     * @brief Emitted whenever the chat list changes (add, remove, clear).
     */
    void chatsChanged();

    /**
     * @brief Emitted when the selected chat changes (including deselection).
     */
    void selectedChatChanged();

private slots:
    /**
     * @brief Handles login state changes.
     *
     * When a user logs in, the chat list is fetched.
     * When a user logs out, the chat list is cleared.
     */
    void onLoggedInChanged();

    /**
     * @brief Processes the incoming list of chats from the server.
     * @param chats The DTO containing chat information from the backend.
     *
     * Updates the internal storage with the received data.
     */
    void onChatListReceived(const shared::ChatsInfo& chats);

    void onChatKeyReceived(const shared::ChatKeyInfo& chatKeyInfo);

private:
    /**
     * @brief Constructs the ChatManager (private for singleton).
     * @param parent Optional QObject parent.
     */
    explicit ChatManager(QObject* parent = nullptr);

    /**
     * @brief Requests the latest chat list from the backend.
     *
     * Typically called after a successful login.
     */
    void fetchChatList();

    /**
     * @brief Clears all chats and resets the selection.
     *
     * Deletes all Chat objects, empties the list and hash map,
     * and emits chatsChanged() and selectedChatChanged() if needed.
     */
    void clearChatList();
};