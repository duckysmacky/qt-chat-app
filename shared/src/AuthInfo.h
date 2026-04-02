#pragma once

#include <QByteArray>
#include <QString>

#include <optional>

namespace shared {

class LoginInfo
{
public:
    LoginInfo();
    LoginInfo(const QString& login, const QString& passwordHash);

    const QString& login() const;
    const QString& passwordHash() const;

    QByteArray serialize() const;
    static std::optional<LoginInfo> deserialize(const QByteArray& bytes);

private:
    QString m_login;
    QString m_passwordHash;
};

class RegisterInfo
{
public:
    RegisterInfo();
    RegisterInfo(const QString& username,
                 const QString& name,
                 const QString& email,
                 const QString& passwordHash);

    const QString& username() const;
    const QString& name() const;
    const QString& email() const;
    const QString& passwordHash() const;

    QByteArray serialize() const;
    static std::optional<RegisterInfo> deserialize(const QByteArray& bytes);

private:
    QString m_username;
    QString m_name;
    QString m_email;
    QString m_passwordHash;
};

}
