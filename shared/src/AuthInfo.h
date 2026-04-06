#pragma once

#include <QByteArray>
#include <QString>

#include <optional>

namespace shared {

class LoginInfo
{
public:
    LoginInfo();
    LoginInfo(QString login, QString passwordHash);

    const QString& login() const { return m_login; }
    const QString& passwordHash() const { return m_passwordHash; }

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
    RegisterInfo(QString username,
                 QString name,
                 QString email,
                 QString passwordHash);

    const QString& username() const { return m_username; }
    const QString& name() const { return m_name; }
    const QString& email() const { return m_email; }
    const QString& passwordHash() const { return m_passwordHash; }

    QByteArray serialize() const;
    static std::optional<RegisterInfo> deserialize(const QByteArray& bytes);

private:
    QString m_username;
    QString m_name;
    QString m_email;
    QString m_passwordHash;
};

}
