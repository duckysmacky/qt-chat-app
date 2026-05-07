#include "PublicUserInfo.h"

#include <QJsonDocument>
#include <QJsonObject>

#include <utility>

namespace shared {

PublicUserInfo::PublicUserInfo() = default;

PublicUserInfo::PublicUserInfo(QUuid userId, QString username, QString displayName)
    : m_userId(std::move(userId)),
      m_username(std::move(username)),
      m_displayName(std::move(displayName))
{
}

QByteArray PublicUserInfo::serialize() const
{
    QJsonObject obj;
    obj["userId"] = m_userId.toString(QUuid::WithoutBraces);
    obj["username"] = m_username;
    obj["displayName"] = m_displayName;

    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

std::optional<PublicUserInfo> PublicUserInfo::deserialize(const QByteArray& bytes)
{
    const QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (!doc.isObject())
        return std::nullopt;

    const QJsonObject obj = doc.object();

    if (!obj.contains("userId") || !obj.contains("username") || !obj.contains("displayName"))
        return std::nullopt;

    return PublicUserInfo(
        QUuid(obj["userId"].toString()),
        obj["username"].toString(),
        obj["displayName"].toString()
    );
}

}
