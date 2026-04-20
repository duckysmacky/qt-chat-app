#pragma once

#include <QObject>

/**
 * @class AccountManager
 * @brief Handles user authentication state and account-related actions.
 *
 * Provides login, registration, logout functionality and tracks
 * current authentication state for UI binding via Qt properties.
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
     * @brief UI mode of the account system.
     */
    enum Mode
    {
        LoginMode,
        RegisterMode,
        AccountMode
    };

    Q_ENUM(Mode)

private:
    /**
     * @brief Pending network action type.
     */
    enum class PendingAction
    {
        None,
        Login,
        Register
    };

private:
    Mode m_mode;                 ///< Current UI mode
    bool m_loggedIn;            ///< Authentication state
    bool m_busy;                ///< Indicates ongoing network request
    PendingAction m_pendingAction; ///< Current pending action
    QString m_pendingUser;      ///< Temporary stored username for login
    QString m_currentUser;      ///< Currently logged-in user
    QString m_statusText;       ///< Status or error message for UI

public:
    /**
     * @brief Returns singleton instance.
     */
    static AccountManager& instance();

    AccountManager(const AccountManager&) = delete;
    AccountManager& operator=(const AccountManager&) = delete;
    AccountManager(AccountManager&&) = delete;
    AccountManager& operator=(AccountManager&&) = delete;

    /**
     * @brief Switches UI to login screen.
     */
    Q_INVOKABLE void showLogin();

    /**
     * @brief Switches UI to registration screen.
     */
    Q_INVOKABLE void showRegister();

    /**
     * @brief Sends login request.
     * @param login Username or login.
     * @param password Plain password.
     */
    Q_INVOKABLE void login(const QString& login, const QString& password);

    /**
     * @brief Sends registration request.
     * @param username Username.
     * @param name Display name.
     * @param email Email address.
     * @param password Plain password.
     */
    Q_INVOKABLE void registerAccount(const QString& username,
                                     const QString& name,
                                     const QString& email,
                                     const QString& password);

    /**
     * @brief Logs out current user.
     */
    Q_INVOKABLE void logout();

    /**
     * @brief Checks if user can send messages.
     * @return true if connected and logged in.
     */
    bool canSendMessages() const;

    /// @return Current UI mode.
    Mode mode() const { return m_mode; }

    /// @return true if user is authenticated.
    bool loggedIn() const { return m_loggedIn; }

    /// @return true if network operation is in progress.
    bool busy() const { return m_busy; }

    /// @return Current username.
    const QString& currentUser() const { return m_currentUser; }

    /// @return Status message (error/info/success).
    const QString& statusText() const { return m_statusText; }

signals:
    void modeChanged();
    void loggedInChanged();
    void busyChanged();
    void canSendMessagesChanged();
    void currentUserChanged();
    void statusTextChanged();

private slots:
    void onConnectionStatusChanged();
    void onResultReceived(bool success, const QString& message);

private:
    explicit AccountManager(QObject* parent = nullptr);

    void setMode(Mode mode);
    void setLoggedIn(bool loggedIn);
    void setBusy(bool busy);
    void setCurrentUser(QString currentUser);
    void setStatusText(QString statusText);
    void resetAuthorizationState();
};