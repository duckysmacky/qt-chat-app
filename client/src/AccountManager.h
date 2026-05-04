#pragma once

#include <QObject>
#include <QUuid>

#include <optional>

#include "OperationResult.h"

/**
 * @class AccountManager
 * @brief Manages user account operations including login, registration, and logout.
 *        This class is a singleton and exposes properties and methods for QML integration.
 */
class AccountManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool canSendMessages READ canSendMessages NOTIFY canSendMessagesChanged)
    Q_PROPERTY(Mode mode READ mode NOTIFY modeChanged)
    Q_PROPERTY(bool loggedIn READ loggedIn NOTIFY loggedInChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QString currentUser READ currentUser NOTIFY currentUserChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)

public:
    /**
     * @enum Mode
     * @brief Represents the current UI mode of the account manager.
     */
    enum Mode
    {
        LoginMode,     ///< Login screen mode
        RegisterMode,  ///< Registration screen mode
        AccountMode    ///< Account management mode (logged in)
    };
    Q_ENUM(Mode)

private:
    /**
     * @enum PendingAction
     * @brief Represents a pending asynchronous action.
     */
    enum class PendingAction
    {
        None,     ///< No pending action
        Login,    ///< Login action pending
        Register, ///< Registration action pending
        Logout
    };

private:
    std::optional<QUuid> m_userId;
    Mode m_mode;                      ///< Current UI mode
    bool m_loggedIn;                  ///< Whether the user is logged in
    bool m_busy;                      ///< Whether an async operation is in progress
    PendingAction m_pendingAction;    ///< Currently pending action
    QString m_pendingUser;            ///< Username of the pending action
    QString m_currentUser;            ///< Currently logged-in username

public:
    /**
     * @brief Returns the singleton instance of AccountManager.
     * @return Reference to the AccountManager instance.
     */
    static AccountManager& instance();

    /// @brief Deleted copy constructor.
    AccountManager(const AccountManager&) = delete;

    /// @brief Deleted copy assignment operator.
    AccountManager& operator=(const AccountManager&) = delete;

    /// @brief Deleted move constructor.
    AccountManager(AccountManager&&) = delete;

    /// @brief Deleted move assignment operator.
    AccountManager& operator=(AccountManager&&) = delete;

    /// @brief Switches the UI to login mode.
    Q_INVOKABLE void showLogin();

    /// @brief Switches the UI to registration mode.
    Q_INVOKABLE void showRegister();

    /**
     * @brief Attempts to log in a user.
     * @param login The username or email.
     * @param password The plain text password (will be hashed).
     */
    Q_INVOKABLE void login(const QString& login, const QString& password);

    /**
     * @brief Attempts to register a new user account.
     * @param username Desired username.
     * @param name Display name.
     * @param email Email address.
     * @param password Plain text password (will be hashed).
     */
    Q_INVOKABLE void registerAccount(const QString& username, const QString& name, const QString& email, const QString& password);

    /// @brief Logs out the current user.
    Q_INVOKABLE void logout();

    /**
     * @brief Checks if the current user can send messages.
     * @return true if the user is logged in and not busy, false otherwise.
     */
    bool canSendMessages() const;

    const std::optional<QUuid>& userId() const { return m_userId; }

    /// @brief Returns the current UI mode.
    Mode mode() const { return m_mode; }

    /// @brief Returns whether a user is logged in.
    bool loggedIn() const { return m_loggedIn; }

    /// @brief Returns whether an asynchronous operation is in progress.
    bool busy() const { return m_busy; }

    /// @brief Returns the username of the currently logged-in user.
    const QString& currentUser() const { return m_currentUser; }

    /// @brief Returns the current status text (e.g., error or info message).
    const QString& statusText() const { return m_statusText; }

signals:
    void modeChanged();               ///< Emitted when the UI mode changes.
    void loggedInChanged();           ///< Emitted when login state changes.
    void busyChanged();               ///< Emitted when busy state changes.
    void canSendMessagesChanged();    ///< Emitted when message-sending permission changes.
    void currentUserChanged();        ///< Emitted when the current user changes.
    void statusTextChanged();         ///< Emitted when the status text changes.

private slots:
    void onConnectionStatusChanged();  ///< Handles changes in server connection status.
    void onOperationResultReceived(const shared::OperationResult& result);  ///< Handles result from async operations.

private:
    /**
     * @brief Private constructor (singleton pattern).
     * @param parent Parent QObject (default nullptr).
     */
    explicit AccountManager(QObject* parent = nullptr);

    void setMode(Mode mode);                     ///< Sets the UI mode and emits modeChanged.
    void setLoggedIn(bool loggedIn);             ///< Sets login state and emits loggedInChanged.
    void setBusy(bool busy);                     ///< Sets busy state and emits busyChanged.
    void setCurrentUser(QString currentUser);    ///< Sets current user and emits currentUserChanged.
    void setStatusText(QString statusText);      ///< Sets status text and emits statusTextChanged.
    void resetAuthorizationState();              ///< Resets authorization-related state.

private:
    QString m_statusText;  ///< Current status message text.
};
