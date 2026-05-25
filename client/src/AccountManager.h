/**
 * @file AccountManager.h
 * @brief Definition of the AccountManager singleton class for user account operations.
 */

#pragma once

#include <QObject>
#include <QUuid>

#include <optional>

#include "OperationResult.h"
#include "dto/ProfileInfo.h"

/**
 * @class AccountManager
 * @brief Manages user account operations including login, registration, and logout.
 *        This class is a singleton and exposes properties and methods for QML integration.
 * 
 * The AccountManager handles all user account-related functionality including authentication,
 * profile management, and UI state management. It provides QML-invokable methods for
 * login, registration, and logout operations, and exposes properties for UI binding.
 */
class AccountManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool canSendMessages READ canSendMessages NOTIFY canSendMessagesChanged)
    Q_PROPERTY(Mode mode READ mode NOTIFY modeChanged)
    Q_PROPERTY(bool loggedIn READ loggedIn NOTIFY loggedInChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(bool userProfileLoaded READ userProfileLoaded NOTIFY userProfileChanged)
    Q_PROPERTY(QString profileUserId READ profileUserId NOTIFY userProfileChanged)
    Q_PROPERTY(QString profileUsername READ profileUsername NOTIFY userProfileChanged)
    Q_PROPERTY(QString profileDisplayName READ profileDisplayName NOTIFY userProfileChanged)
    Q_PROPERTY(QString profileEmail READ profileEmail NOTIFY userProfileChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)

public:
    /**
     * @enum Mode
     * @brief Represents the current UI mode of the account manager.
     */
    enum Mode
    {
        LoginMode,     ///< Login screen mode - user is prompted for credentials.
        RegisterMode,  ///< Registration screen mode - user can create a new account.
        AccountMode    ///< Account management mode - user is logged in and can manage profile.
    };
    Q_ENUM(Mode)

private:
    /**
     * @enum PendingAction
     * @brief Represents a pending asynchronous action.
     * 
     * Tracks which async operation is currently in progress to properly handle
     * responses and prevent multiple simultaneous operations.
     */
    enum class PendingAction
    {
        None,          ///< No pending action.
        Register,      ///< Registration action is pending.
        Login,         ///< Login action is pending.
        FetchProfile,  ///< Profile fetch action is pending.
        Logout         ///< Logout action is pending.
    };

private:
    std::optional<shared::ProfileInfo> m_userProfile;  ///< Current user's profile information, if loaded.
    Mode m_mode;                                        ///< Current UI mode (Login/Register/Account).
    bool m_loggedIn;                                    ///< Whether the user is currently logged in.
    bool m_busy;                                        ///< Whether an asynchronous operation is in progress.
    PendingAction m_pendingAction;                      ///< Currently pending action type.
    QString m_pendingLoginPasswordHash;                 ///< Password hash retained until profile loads and local keystore unlocks.
    QString m_statusText;                               ///< Current status message text (e.g., error or info).


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

    /**
     * @brief Switches the UI to login mode.
     * 
     * Sets the current mode to LoginMode, allowing the user to enter credentials.
     */
    Q_INVOKABLE void showLogin();

    /**
     * @brief Switches the UI to registration mode.
     * 
     * Sets the current mode to RegisterMode, allowing the user to create a new account.
     */
    Q_INVOKABLE void showRegister();

    /**
     * @brief Attempts to log in a user.
     * @param login The username or email address.
     * @param password The plain text password (will be hashed before transmission).
     * 
     * Initiates an asynchronous login request. The result is handled via
     * onOperationResultReceived() signal.
     */
    Q_INVOKABLE void login(const QString& login, const QString& password);

    /**
     * @brief Attempts to register a new user account.
     * @param username Desired unique username.
     * @param displayName Display name shown to other users.
     * @param email Email address for the account.
     * @param password Plain text password (will be hashed before transmission).
     * 
     * Initiates an asynchronous registration request. The result is handled via
     * onOperationResultReceived() signal.
     */
    Q_INVOKABLE void registerAccount(const QString& username, const QString& displayName, const QString& email, const QString& password);

    /**
     * @brief Logs out the current user.
     * 
     * Clears the current session and resets the UI to login mode.
     */
    Q_INVOKABLE void logout();

    /**
     * @brief Checks if the current user can send messages.
     * @return true if the user is logged in and not busy, false otherwise.
     * 
     * Used to disable message input controls during async operations.
     */
    bool canSendMessages() const;

    /**
     * @brief Checks if the user profile has been loaded.
     * @return true if profile data is available, false otherwise.
     */
    bool userProfileLoaded() const { return m_userProfile.has_value(); }

    /**
     * @brief Returns the current user's profile user ID.
     * @return QString containing the user ID, or empty string if not loaded.
     */
    QString profileUserId() const;
    
    /**
     * @brief Returns the current user's username.
     * @return QString containing the username, or empty string if not loaded.
     */
    QString profileUsername() const;
    
    /**
     * @brief Returns the current user's display name.
     * @return QString containing the display name, or empty string if not loaded.
     */
    QString profileDisplayName() const;
    
    /**
     * @brief Returns the current user's email address.
     * @return QString containing the email, or empty string if not loaded.
     */
    QString profileEmail() const;

    /**
     * @brief Returns the current user's profile information.
     * @return std::optional<shared::ProfileInfo> containing the profile if loaded.
     */
    const std::optional<shared::ProfileInfo>& userProfile() const { return m_userProfile; }

    /**
     * @brief Returns the current user's ID as a QUuid.
     * @return std::optional<QUuid> containing the user ID if available.
     */
    std::optional<QUuid> userId() const;

    /**
     * @brief Returns the current UI mode.
     * @return Mode enum value indicating the current screen.
     */
    Mode mode() const { return m_mode; }

    /**
     * @brief Returns whether a user is logged in.
     * @return true if logged in, false otherwise.
     */
    bool loggedIn() const { return m_loggedIn; }

    /**
     * @brief Returns whether an asynchronous operation is in progress.
     * @return true if busy, false otherwise.
     * 
     * Used to disable UI controls during network operations.
     */
    bool busy() const { return m_busy; }

    /**
     * @brief Returns the current status text (e.g., error or info message).
     * @return Constant reference to the status text string.
     */
    const QString& statusText() const { return m_statusText; }

signals:
    void modeChanged();               ///< Emitted when the UI mode changes.
    void loggedInChanged();           ///< Emitted when login state changes.
    void busyChanged();               ///< Emitted when busy state changes.
    void canSendMessagesChanged();    ///< Emitted when message-sending permission changes.
    void userProfileChanged();        ///< Emitted when the current user's profile changes.
    void statusTextChanged();         ///< Emitted when the status text changes.

private slots:
    /**
     * @brief Handles changes in server connection status.
     * 
     * Updates UI state based on connection availability.
     */
    void onConnectionStatusChanged();
    
    /**
     * @brief Handles result from asynchronous operations.
     * @param result The OperationResult containing success/failure information.
     * 
     * Processes responses from login, registration, logout, and profile fetch operations.
     */
    void onOperationResultReceived(const shared::OperationResult& result);
    
    /**
     * @brief Handles receipt of the current user's profile.
     * @param profile The ProfileInfo containing user profile data.
     * 
     * Stores the profile and updates the UI state.
     */
    void onCurrentUserProfileReceived(const shared::ProfileInfo& profile);

private:
    /**
     * @brief Private constructor (singleton pattern).
     * @param parent Parent QObject (default nullptr).
     */
    explicit AccountManager(QObject* parent = nullptr);

    /**
     * @brief Sets the UI mode and emits modeChanged.
     * @param mode The new Mode value.
     */
    void setMode(Mode mode);
    
    /**
     * @brief Sets login state and emits loggedInChanged.
     * @param loggedIn The new login state.
     */
    void setLoggedIn(bool loggedIn);
    
    /**
     * @brief Sets busy state and emits busyChanged.
     * @param busy The new busy state.
     */
    void setBusy(bool busy);
    
    /**
     * @brief Sets status text and emits statusTextChanged.
     * @param statusText The new status message text.
     */
    void setStatusText(QString statusText);
    
    /**
     * @brief Sets the user profile and emits userProfileChanged.
     * @param profile The new profile information (or nullopt to clear).
     */
    void setUserProfile(std::optional<shared::ProfileInfo> profile);
    
    /**
     * @brief Resets authorization-related state.
     * 
     * Clears user profile, sets loggedIn to false, and switches to LoginMode.
     */
    void resetAuthorizationState();
};
