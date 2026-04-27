#pragma once

#include <QByteArray>
#include <QString>

#include <optional>

namespace shared {

class ProfileUpdateInfo
{
public:
    ProfileUpdateInfo() = default;

    const std::optional<QString>& username() const { return m_username; }
    const std::optional<QString>& name() const { return m_name; }
    const std::optional<QString>& email() const { return m_email; }
    const std::optional<QString>& passwordHash() const { return m_passwordHash; }

    void setUsername(QString username) { m_username = std::move(username); }
    void setName(QString name) { m_name = std::move(name); }
    void setEmail(QString email) { m_email = std::move(email); }
    void setPasswordHash(QString passwordHash) { m_passwordHash = std::move(passwordHash); }

    bool isEmpty() const;

    QByteArray serialize() const;
    static std::optional<ProfileUpdateInfo> deserialize(const QByteArray& bytes);

private:
    std::optional<QString> m_username;
    std::optional<QString> m_name;
    std::optional<QString> m_email;
    std::optional<QString> m_passwordHash;
};

}
