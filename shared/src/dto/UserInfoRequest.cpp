#include "UserInfoRequest.h"

#include <QJsonDocument>
#include <QJsonObject>

#include <utility>

namespace shared {

namespace {

QString identifierTypeToString(const UserIdentifierType type)
{
    switch (type)
    {
    case UserIdentifierType::UUID:     return "uuid";
    case UserIdentifierType::USERNAME: return "username";
    }

    return "uuid";
}

std::optional<UserIdentifierType> identifierTypeFromString(const QString& type)
{
    if (type == "uuid")
        return UserIdentifierType::UUID;

    if (type == "username")
        return UserIdentifierType::USERNAME;

    return std::nullopt;
}

}

UserInfoRequest::UserInfoRequest()
    : m_identifierType(UserIdentifierType::UUID)
{
}

UserInfoRequest::UserInfoRequest(QUuid userId)
    : m_identifierType(UserIdentifierType::UUID),
      m_userId(std::move(userId))
{
}

UserInfoRequest::UserInfoRequest(QString username)
    : m_identifierType(UserIdentifierType::USERNAME),
      m_username(std::move(username))
{
}

QByteArray UserInfoRequest::serialize() const
{
    QJsonObject obj;
    obj["identifierType"] = identifierTypeToString(m_identifierType);

    switch (m_identifierType)
    {
    case UserIdentifierType::UUID:
        obj["userId"] = m_userId.toString(QUuid::WithoutBraces);
        break;
    case UserIdentifierType::USERNAME:
        obj["username"] = m_username;
        break;
    }

    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

std::optional<UserInfoRequest> UserInfoRequest::deserialize(const QByteArray& bytes)
{
    const QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (!doc.isObject())
        return std::nullopt;

    const QJsonObject obj = doc.object();
    if (!obj.contains("identifierType"))
        return std::nullopt;

    const auto identifierType = identifierTypeFromString(obj["identifierType"].toString());
    if (!identifierType.has_value())
        return std::nullopt;

    switch (identifierType.value())
    {
    case UserIdentifierType::UUID:
        if (!obj.contains("userId"))
            return std::nullopt;

        return UserInfoRequest(QUuid(obj["userId"].toString()));

    case UserIdentifierType::USERNAME:
        if (!obj.contains("username"))
            return std::nullopt;

        return UserInfoRequest(obj["username"].toString());
    }

    return std::nullopt;
}

}
