#include "AccountManager.h"

#include <QDebug>

#include "Client.h"
#include "Hasher.h"
#include "RequestManager.h"

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
    connect(&client, &Client::connectionStatusChanged, this, &AccountManager::onConnectionStatusChanged);
    connect(&RequestManager::instance(), &RequestManager::resultReceived, this, &AccountManager::onResultReceived);
}

void AccountManager::showLogin()
{
    if (m_busy) return;
    setStatusText("");
    setMode(m_loggedIn ? AccountMode : LoginMode);
}

void AccountManager::showRegister()
{
    if (m_busy || m_loggedIn) return;
    setStatusText("");
    setMode(RegisterMode);
}

void AccountManager::login(const QString& login, const QString& password)
{
    if (m_busy || !Client::instance().connected()) return;
    if (login.trimmed().isEmpty() || password.isEmpty()) return;

    m_pendingAction = PendingAction::Login;
    m_pendingUser = login.trimmed();
    setStatusText("");
    setBusy(true);

    RequestManager::instance().loginUser(m_pendingUser, Hasher::sha256(password));
}

void AccountManager::registerAccount(const QString& username, const QString& name, const QString& email, const QString& password)
{
    if (m_busy || !Client::instance().connected()) return;
    if (username.trimmed().isEmpty() || name.trimmed().isEmpty() || email.trimmed().isEmpty() || password.isEmpty())
        return;

    m_pendingAction = PendingAction::Register;
    setStatusText("");
    setBusy(true);

    RequestManager::instance().registerUser(
        username.trimmed(),
        name.trimmed(),
        email.trimmed(),
        Hasher::sha256(password)
    );
}

void AccountManager::logout()
{
    if (m_busy || !Client::instance().connected()) return;
    if (!m_loggedIn) return;

    m_pendingAction = PendingAction::Logout;
    setStatusText("");
    setBusy(true);

    RequestManager::instance().logoutUser();
}

bool AccountManager::canSendMessages() const
{
    return Client::instance().connected() && m_loggedIn;
}

void AccountManager::onConnectionStatusChanged()
{
    if (Client::instance().connected()) return;

    setBusy(false);
    m_pendingAction = PendingAction::None;
    m_pendingUser.clear();
    resetAuthorizationState();
}

void AccountManager::onResultReceived(const shared::OperationResult& result)
{
    const bool success = result.type() == shared::OperationResultType::SUCCESS;
    const QString& message = result.text();

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

    case PendingAction::Logout:
        setBusy(false);
        m_pendingAction = PendingAction::None;

        if (!success)
        {
            qWarning() << "Logout failed:" << message;
            setStatusText(message);
            return;
        }

        qInfo() << "Logout succeeded:" << message;
        resetAuthorizationState();
        break;

    case PendingAction::None:
        break;
    }
}

void AccountManager::setMode(const Mode mode)
{
    if (m_mode == mode) return;
    m_mode = mode;
    emit modeChanged();
}

void AccountManager::setLoggedIn(const bool loggedIn)
{
    if (m_loggedIn == loggedIn) return;
    m_loggedIn = loggedIn;
    emit loggedInChanged();
    emit canSendMessagesChanged();
}

void AccountManager::setBusy(const bool busy)
{
    if (m_busy == busy) return;
    m_busy = busy;
    emit busyChanged();
}

void AccountManager::setCurrentUser(QString currentUser)
{
    if (m_currentUser == currentUser) return;
    m_currentUser = std::move(currentUser);
    emit currentUserChanged();
}

void AccountManager::setStatusText(QString statusText)
{
    if (m_statusText == statusText) return;

    m_statusText = std::move(statusText);
    emit statusTextChanged();
}

void AccountManager::resetAuthorizationState()
{
    setLoggedIn(false);
    setCurrentUser("");
    setStatusText("");
    setMode(LoginMode);
}
