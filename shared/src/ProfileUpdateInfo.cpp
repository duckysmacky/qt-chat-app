#include "ProfileUpdateInfo.h"

#include <QJsonDocument>
#include <QJsonObject>

namespace shared {

bool ProfileUpdateInfo::isEmpty() const
{
    return !m_username.has_value()
    && !m_name.has_value()
        && !m_email.has_value()
        && !m_passwordHash.has_value();
}

QByteArray ProfileUpdateInfo::serialize() const
{
    QJsonObject obj;

    if (m_username.has_value()) obj["username"] = m_username.value();
    if (m_name.has_value()) obj["name"] = m_name.value();
    if (m_email.has_value()) obj["email"] = m_email.value();
    if (m_passwordHash.has_value()) obj["passwordHash"] = m_passwordHash.value();

    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

std::optional<ProfileUpdateInfo> ProfileUpdateInfo::deserialize(const QByteArray& bytes)
{
    const QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (!doc.isObject())
        return std::nullopt;

    const QJsonObject obj = doc.object();

    ProfileUpdateInfo info;

    if (obj.contains("username")) info.setUsername(obj["username"].toString());
    if (obj.contains("name")) info.setName(obj["name"].toString());
    if (obj.contains("email")) info.setEmail(obj["email"].toString());
    if (obj.contains("passwordHash")) info.setPasswordHash(obj["passwordHash"].toString());

    if (info.isEmpty())
        return std::nullopt;

    return info;
}

}
