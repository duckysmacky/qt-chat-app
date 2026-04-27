#pragma once

#include <QByteArray>
#include <QString>
#include <QUuid>

#include <optional>

namespace shared {

class ProfileInfo
{
public:
    ProfileInfo();
    ProfileInfo(QUuid userId, QString username, QString name, QString email);

    const QUuid& userId() const { return m_userId; }
    const QString& username() const { return m_username; }
    const QString& name() const { return m_name; }
    const QString& email() const { return m_email; }

    QByteArray serialize() const;
    static std::optional<ProfileInfo> deserialize(const QByteArray& bytes);

private:
    QUuid m_userId;
    QString m_username;
    QString m_name;
    QString m_email;
};

}
