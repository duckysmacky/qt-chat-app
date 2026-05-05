#include "AccountManager.h"

#include <QDebug>

#include <utility>

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
    const Client& client = Client::instance();
    const RequestManager& requestManager = RequestManager::instance();

    connect(&client, &Client::connectionStatusChanged, this, &AccountManager::onConnectionStatusChanged);
    connect(&requestManager, &RequestManager::operationResultReceived, this, &AccountManager::onOperationResultReceived);
    connect(&requestManager, &RequestManager::currentUserProfileReceived, this, &AccountManager::onCurrentUserProfileReceived);
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
    setUserProfile(std::nullopt);
    setStatusText("");
    setBusy(true);

    RequestManager::instance().loginUser(login.trimmed(), Hasher::sha256(password));
}

void AccountManager::registerAccount(const QString& username, const QString& name, const QString& email, const QString& password)
{
    if (m_busy || !Client::instance().connected()) return;
    if (username.trimmed().isEmpty() || name.trimmed().isEmpty() || email.trimmed().isEmpty() || password.isEmpty())
        return;

    m_pendingAction = PendingAction::Register;
    setUserProfile(std::nullopt);
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

    RequestManager::instance().logoutCurrentUser();
}

bool AccountManager::canSendMessages() const
{
    return Client::instance().connected() && m_loggedIn;
}

std::optional<QUuid> AccountManager::userId() const
{
    if (!m_userProfile.has_value())
        return std::nullopt;

    return m_userProfile->userId();
}

void AccountManager::onConnectionStatusChanged()
{
    if (Client::instance().connected()) return;

    setBusy(false);
    m_pendingAction = PendingAction::None;
    resetAuthorizationState();
}

void AccountManager::onOperationResultReceived(const shared::OperationResult& result)
{
    const bool success = result.type() == shared::OperationResultType::SUCCESS;
    const QString& message = result.text();

    switch (m_pendingAction)
    {
    case PendingAction::None:
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

    case PendingAction::Login:
        if (!success)
        {
            setBusy(false);
            m_pendingAction = PendingAction::None;
            qWarning() << "Login failed:" << message;
            setStatusText(message);
            return;
        }

        qInfo() << "Login accepted:" << message;
        m_pendingAction = PendingAction::FetchProfile;
        setStatusText("");
        setMode(AccountMode);
        RequestManager::instance().getCurrentUserProfile();
        break;

    case PendingAction::FetchProfile:
        if (!success)
        {
            setBusy(false);
            m_pendingAction = PendingAction::None;
            qWarning() << "Profile request failed:" << message;
            resetAuthorizationState();
            setStatusText(message);
        }
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
    }
}

void AccountManager::onCurrentUserProfileReceived(const shared::ProfileInfo& profile)
{
    if (m_pendingAction != PendingAction::FetchProfile && !m_loggedIn)
        return;

    setUserProfile(profile);

    if (m_pendingAction != PendingAction::FetchProfile)
        return;

    qInfo() << "Loaded profile for logged-in user:" << profile.username();
    m_pendingAction = PendingAction::None;
    setStatusText("");
    setLoggedIn(true);
    setMode(AccountMode);
    setBusy(false);
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

void AccountManager::setStatusText(QString statusText)
{
    if (m_statusText == statusText) return;

    m_statusText = std::move(statusText);
    emit statusTextChanged();
}

QString AccountManager::profileUserId() const
{
    if (!m_userProfile.has_value())
        return "";

    return m_userProfile->userId().toString(QUuid::WithoutBraces);
}

QString AccountManager::profileUsername() const
{
    if (!m_userProfile.has_value())
        return "";

    return m_userProfile->username();
}

QString AccountManager::profileName() const
{
    if (!m_userProfile.has_value())
        return "";

    return m_userProfile->name();
}

QString AccountManager::profileEmail() const
{
    if (!m_userProfile.has_value())
        return "";

    return m_userProfile->email();
}

void AccountManager::setUserProfile(std::optional<shared::ProfileInfo> profile)
{
    m_userProfile = std::move(profile);
    emit userProfileChanged();
}

void AccountManager::resetAuthorizationState()
{
    setUserProfile(std::nullopt);
    setLoggedIn(false);
    setStatusText("");
    setMode(LoginMode);
}
