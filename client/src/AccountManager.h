#pragma once

#include <QObject>

class AccountManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool canSendMessages READ canSendMessages NOTIFY canSendMessagesChanged)
    Q_PROPERTY(Mode mode READ mode NOTIFY modeChanged)
    Q_PROPERTY(bool loggedIn READ loggedIn NOTIFY loggedInChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QString currentUser READ currentUser NOTIFY currentUserChanged)

public:
    enum Mode
    {
        LoginMode,
        RegisterMode,
        AccountMode
    };

    Q_ENUM(Mode)

private:
    enum class PendingAction
    {
        None,
        Login,
        Register
    };

private:
    Mode m_mode;
    bool m_loggedIn;
    bool m_busy;
    PendingAction m_pendingAction;
    QString m_pendingUser;
    QString m_currentUser;

public:
    static AccountManager& instance();

    AccountManager(const AccountManager&) = delete;
    AccountManager& operator=(const AccountManager&) = delete;
    AccountManager(AccountManager&&) = delete;
    AccountManager& operator=(AccountManager&&) = delete;

    Q_INVOKABLE void showLogin();
    Q_INVOKABLE void showRegister();
    Q_INVOKABLE void login(const QString& login, const QString& password);
    Q_INVOKABLE void registerAccount(const QString& username, const QString& name, const QString& email, const QString& password);
    Q_INVOKABLE void logout();

    bool canSendMessages() const;
    
	Mode mode() const { return m_mode; }
    bool loggedIn() const { return m_loggedIn; }
    bool busy() const { return m_busy; }
    const QString& currentUser() const { return m_currentUser; }

signals:
    void modeChanged();
    void loggedInChanged();
    void busyChanged();
    void canSendMessagesChanged();
    void currentUserChanged();

private slots:
    void onConnectionStatusChanged();
    void onResultReceived(bool success, const QString& message);

private:
    explicit AccountManager(QObject* parent = nullptr);

    void setMode(Mode mode);
    void setLoggedIn(bool loggedIn);
    void setBusy(bool busy);
    void setCurrentUser(QString currentUser);
    void resetAuthorizationState();
};
