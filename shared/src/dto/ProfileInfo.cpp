#include "ProfileInfo.h"

#include <QJsonDocument>
#include <QJsonObject>

#include <utility>

namespace shared {

ProfileInfo::ProfileInfo() = default;

ProfileInfo::ProfileInfo(QUuid userId, QString username, QString displayName, QString email)
    : m_userId(std::move(userId)),
    m_username(std::move(username)),
    m_displayName(std::move(displayName)),
    m_email(std::move(email))
{
}

QByteArray ProfileInfo::serialize() const
{
    QJsonObject obj;
    obj["userId"] = m_userId.toString(QUuid::WithoutBraces);
    obj["username"] = m_username;
    obj["displayName"] = m_displayName;
    obj["email"] = m_email;

    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

std::optional<ProfileInfo> ProfileInfo::deserialize(const QByteArray& bytes)
{
    const QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (!doc.isObject())
        return std::nullopt;

    const QJsonObject obj = doc.object();

    if (!obj.contains("userId") || !obj.contains("username") || !obj.contains("displayName") || !obj.contains("email"))
        return std::nullopt;

    return ProfileInfo(
        QUuid(obj["userId"].toString()),
        obj["username"].toString(),
        obj["displayName"].toString(),
        obj["email"].toString()
        );
}

}
