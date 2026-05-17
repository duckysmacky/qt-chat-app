/**
 * @file ChatCreator.h
 * @brief Manages the creation of group chats and addition of members.
 */

#pragma once

#include <QList>
#include <QObject>
#include <QSet>
#include <QString>
#include <QUuid>
#include <QVariantList>

#include <optional>

#include "OperationResult.h"
#include "dto/ChatInfo.h"
#include "dto/PublicUserInfo.h"

/**
 * @class ChatCreator
 * @brief Singleton class responsible for creating chats and managing member selection.
 * 
 * The ChatCreator class provides functionality to add users by username, manage a list
 * of selected members, and create a new chat. It exposes properties and invokable methods
 * for QML integration and emits signals when state changes.
 */
class ChatCreator : public QObject
{
    Q_OBJECT
    /**
     * @property members
     * @brief List of selected members for the new chat.
     * 
     * Accessible from QML as a QVariantList containing PublicUserInfo objects.
     * Emits membersChanged() when updated.
     */
    Q_PROPERTY(QVariantList members READ members NOTIFY membersChanged)
    
    /**
     * @property statusText
     * @brief Current status message for user feedback.
     * 
     * Displays information about operations like user resolution or chat creation.
     * Emits statusTextChanged() when updated.
     */
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    
    /**
     * @property canCreateChat
     * @brief Indicates whether a chat can be created.
     * 
     * Returns true when at least one member is selected, no pending user resolution
     * is in progress, and chat creation is not already ongoing.
     * Emits canCreateChatChanged() when state changes.
     */
    Q_PROPERTY(bool canCreateChat READ canCreateChat NOTIFY canCreateChatChanged)
    
    /**
     * @property resolving
     * @brief Indicates whether a username resolution is in progress.
     * 
     * Returns true while waiting for a user to be resolved from a username.
     * Emits resolvingChanged() when state changes.
     */
    Q_PROPERTY(bool resolving READ resolving NOTIFY resolvingChanged)

private:
    QList<shared::PublicUserInfo> m_members;      ///< List of selected members
    QSet<QUuid> m_memberIds;                       ///< Set of member UUIDs for fast lookup
    std::optional<QString> m_pendingUsername;      ///< Username currently being resolved
    QString m_statusText;                          ///< Current status text
    bool m_creating;                               ///< Flag indicating chat creation is in progress

public:
    /**
     * @brief Gets the singleton instance of ChatCreator.
     * @return Reference to the singleton ChatCreator instance.
     */
    static ChatCreator& instance();

    // Delete copy and move constructors/operators to enforce singleton pattern
    ChatCreator(const ChatCreator&) = delete;
    ChatCreator& operator=(const ChatCreator&) = delete;
    ChatCreator(ChatCreator&&) = delete;
    ChatCreator& operator=(ChatCreator&&) = delete;

    /**
     * @brief Adds a user to the chat by username.
     * @param usernameText The username text to resolve and add.
     * 
     * This method resolves the username asynchronously. If successful, the user is
     * added to the members list. If already present or resolution fails, appropriate
     * status text is set.
     */
    Q_INVOKABLE void addUser(const QString& usernameText);
    
    /**
     * @brief Creates a new chat with the selected members.
     * 
     * Initiates the chat creation process. Sets the creating flag to true and
     * emits state changes. The result is handled asynchronously.
     */
    Q_INVOKABLE void createChat();
    
    /**
     * @brief Resets the chat creator state.
     * 
     * Clears all selected members, pending username, status text, and creation flag.
     */
    Q_INVOKABLE void reset();

    /**
     * @brief Gets the list of selected members.
     * @return QVariantList containing PublicUserInfo objects for selected members.
     */
    QVariantList members() const;
    
    /**
     * @brief Gets the current status text.
     * @return const reference to the status text string.
     */
    const QString& statusText() const { return m_statusText; }
    
    /**
     * @brief Checks if a chat can be created.
     * @return True if members exist, no pending resolution, and not already creating.
     */
    bool canCreateChat() const { return !m_memberIds.isEmpty() && !resolving() && !m_creating; }
    
    /**
     * @brief Checks if a username resolution is in progress.
     * @return True if a username is pending resolution.
     */
    bool resolving() const { return m_pendingUsername.has_value(); }

signals:
    /**
     * @brief Emitted when the members list changes.
     */
    void membersChanged();
    
    /**
     * @brief Emitted when the status text changes.
     */
    void statusTextChanged();
    
    /**
     * @brief Emitted when the ability to create a chat changes.
     */
    void canCreateChatChanged();
    
    /**
     * @brief Emitted when the resolving state changes.
     */
    void resolvingChanged();

private:
    /**
     * @brief Private constructor for singleton pattern.
     * @param parent Parent QObject (default nullptr).
     */
    explicit ChatCreator(QObject* parent = nullptr);

    /**
     * @brief Adds a resolved user to the members list.
     * @param userInfo The resolved user information.
     */
    void addResolvedUser(const shared::PublicUserInfo& userInfo);
    
    /**
     * @brief Clears the pending username and updates resolving state.
     */
    void clearPendingUser();
    
    /**
     * @brief Sets the status text and emits the statusTextChanged signal.
     * @param statusText New status text.
     */
    void setStatusText(QString statusText);
    
    /**
     * @brief Emits all state change signals (members, canCreateChat, resolving).
     */
    void emitCreateStateChanged();

    /**
     * @brief Callback invoked when a username is successfully resolved to a user.
     * @param userId UUID of the resolved user.
     * @param userInfo Public user information of the resolved user.
     */
    void onUserResolved(const QUuid& userId, const shared::PublicUserInfo& userInfo);
    
    /**
     * @brief Callback invoked when an operation result is received.
     * @param result The operation result from the chat creation attempt.
     */
    void onOperationResultReceived(const shared::OperationResult& result);
    
    /**
     * @brief Callback invoked when chat information is received after successful creation.
     * @param chatInfo Information about the newly created chat.
     */
    void onChatInfoReceived(const shared::ChatInfo& chatInfo);
};