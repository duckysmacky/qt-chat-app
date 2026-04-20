#include "AccountManager.h"

#include <QDebug>

#include "Client.h"
#include "Hasher.h"

AccountManager& AccountManager::instance()
{
    static AccountManager instance;
    return instance;
}

AccountManager::AccountManager(QObject* parent)
    : QObject(parent),
      m_mode(LoginMode),
      m_loggedIn(false),
      m_busy(false),
      m_pendingAction(PendingAction::None)
{
    Client& client = Client::instance();
    connect(&client, &Client::connectionStatusChanged,
            this, &AccountManager::onConnectionStatusChanged);
    connect(&client, &Client::resultReceived,
            this, &AccountManager::onResultReceived);
}

/**
 * @brief Shows login view or account view depending on current state.
 */
void AccountManager::showLogin()
{
    if (m_busy) return;
    setStatusText("");
    setMode(m_loggedIn ? AccountMode : LoginMode);
}

/**
 * @brief Shows registration view.
 */
void AccountManager::showRegister()
{
    if (m_busy || m_loggedIn) return;
    setStatusText("");
    setMode(RegisterMode);
}

/**
 * @brief Sends login request to the server.
 * @param login Username or login.
 * @param password Plain password (will be hashed).
 */
void AccountManager::login(const QString& login, const QString& password)
{
    if (m_busy || !Client::instance().connected()) return;
    if (login.trimmed().isEmpty() || password.isEmpty()) return;

    m_pendingAction = PendingAction::Login;
    m_pendingUser = login.trimmed();
    setStatusText("");
    setBusy(true);

    Client::instance().login(m_pendingUser, Hasher::sha256(password));
}

/**
 * @brief Sends registration request to the server.
 * @param username Username.
 * @param name Display name.
 * @param email Email address.
 * @param password Plain password (will be hashed).
 */
void AccountManager::registerAccount(const QString& username,
                                     const QString& name,
                                     const QString& email,
                                     const QString& password)
{
    if (m_busy || !Client::instance().connected()) return;
    if (username.trimmed().isEmpty() ||
        name.trimmed().isEmpty() ||
        email.trimmed().isEmpty() ||
        password.isEmpty())
        return;

    m_pendingAction = PendingAction::Register;
    setStatusText("");
    setBusy(true);

    Client::instance().registerUser(
        username.trimmed(),
        name.trimmed(),
        email.trimmed(),
        Hasher::sha256(password)
    );
}

/**
 * @brief Logs out the current user.
 */
void AccountManager::logout()
{
    qInfo() << "Logout pressed";
}

/**
 * @brief Checks whether the user can send messages.
 * @return true if connected and authenticated.
 */
bool AccountManager::canSendMessages() const
{
    return Client::instance().connected() && m_loggedIn;
}

/**
 * @brief Handles connection state changes.
 */
void AccountManager::onConnectionStatusChanged()
{
    emit canSendMessagesChanged();

    if (Client::instance().connected()) return;

    setBusy(false);
    m_pendingAction = PendingAction::None;
    m_pendingUser.clear();
    resetAuthorizationState();
}

/**
 * @brief Handles server response results for login/register actions.
 * @param success Indicates whether operation succeeded.
 * @param message Server message or error description.
 */
void AccountManager::onResultReceived(const bool success, const QString& message)
{
    switch (m_pendingAction)
    {
    case PendingAction::Login:
        setBusy(false);
        m_pendingAction = PendingAction::None;

        if (!success)
        {
            qWarning() << "Login failed:" << message;
            setStatusText(message);
            return;
        }

        qInfo() << "Login succeeded:" << message;
        setStatusText("");
        setCurrentUser(m_pendingUser);
        setLoggedIn(true);
        setMode(AccountMode);
        m_pendingUser.clear();
        break;

    case PendingAction::Register:
        setBusy(false);
        m_pendingAction = PendingAction::None;

        if (!success)
        {
            qWarning() << "Registration failed:" << message;
            setStatusText(message);
            return;
        }

        qInfo() << "Registration succeeded:" << message;
        setStatusText("");
        setMode(LoginMode);
        break;

    case PendingAction::None:
        break;
    }
}

/**
 * @brief Sets current UI mode.
 */
void AccountManager::setMode(const Mode mode)
{
    if (m_mode == mode) return;
    m_mode = mode;
    emit modeChanged();
}

/**
 * @brief Updates login state.
 */
void AccountManager::setLoggedIn(const bool loggedIn)
{
    if (m_loggedIn == loggedIn) return;
    m_loggedIn = loggedIn;
    emit loggedInChanged();
    emit canSendMessagesChanged();
}

/**
 * @brief Sets busy state (e.g., network request in progress).
 */
void AccountManager::setBusy(const bool busy)
{
    if (m_busy == busy) return;
    m_busy = busy;
    emit busyChanged();
}

/**
 * @brief Updates current username.
 */
void AccountManager::setCurrentUser(QString currentUser)
{
    if (m_currentUser == currentUser) return;
    m_currentUser = std::move(currentUser);
    emit currentUserChanged();
}

/**
 * @brief Sets status message (error/success/info text).
 */
void AccountManager::setStatusText(QString statusText)
{
    if (m_statusText == statusText) return;

    m_statusText = std::move(statusText);
    emit statusTextChanged();
}

/**
 * @brief Resets authentication-related state.
 */
void AccountManager::resetAuthorizationState()
{
    setLoggedIn(false);
    setCurrentUser("");
    setStatusText("");
    setMode(LoginMode);
}