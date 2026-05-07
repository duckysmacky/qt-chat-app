#pragma once

#include <QByteArray>
#include <QString>
#include <QUuid>

#include <optional>

namespace shared {

class PublicUserInfo
{
public:
    PublicUserInfo();
    PublicUserInfo(QUuid userId, QString username, QString displayName);

    const QUuid& userId() const { return m_userId; }
    const QString& username() const { return m_username; }
    const QString& displayName() const { return m_displayName; }

    QByteArray serialize() const;
    static std::optional<PublicUserInfo> deserialize(const QByteArray& bytes);

private:
    QUuid m_userId;
    QString m_username;
    QString m_displayName;
};

}
