#include "SessionInfo.h"

#include <QJsonDocument>
#include <QJsonObject>

#include <utility>

namespace shared {

namespace {

constexpr auto userIdKey = "userId";
constexpr auto sessionIdKey = "sessionId";

} // namespace

SessionInfo::SessionInfo(QUuid userId, QUuid sessionId)
    : m_userId(std::move(userId)),
      m_sessionId(std::move(sessionId))
{
}

QByteArray SessionInfo::serialize() const
{
    QJsonObject obj;
    obj[userIdKey] = m_userId.toString(QUuid::WithoutBraces);
    obj[sessionIdKey] = m_sessionId.toString(QUuid::WithoutBraces);

    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

std::optional<SessionInfo> SessionInfo::deserialize(const QByteArray& bytes)
{
    const QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (!doc.isObject())
        return std::nullopt;

    const QJsonObject obj = doc.object();
    if (!obj.contains(userIdKey) || !obj.contains(sessionIdKey))
        return std::nullopt;

    return SessionInfo(
        QUuid(obj[userIdKey].toString()),
        QUuid(obj[sessionIdKey].toString())
    );
}

} // namespace shared
