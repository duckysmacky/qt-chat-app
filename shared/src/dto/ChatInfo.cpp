#include "ChatInfo.h"

#include <QJsonDocument>
#include <QJsonObject>

#include <utility>

namespace shared {

ChatInfo::ChatInfo() = default;

ChatInfo::ChatInfo(QUuid id, QString type, QUuid createdBy, QString title, QDateTime createdAt)
    : m_id(std::move(id)),
    m_type(std::move(type)),
    m_createdBy(std::move(createdBy)),
    m_title(std::move(title)),
    m_createdAt(std::move(createdAt))
{
}

QByteArray ChatInfo::serialize() const
{
    QJsonObject obj;
    obj["id"] = m_id.toString(QUuid::WithoutBraces);
    obj["type"] = m_type;
    obj["createdBy"] = m_createdBy.toString(QUuid::WithoutBraces);
    obj["title"] = m_title;
    obj["createdAt"] = m_createdAt.toUTC().toString(Qt::ISODate);

    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

std::optional<ChatInfo> ChatInfo::deserialize(const QByteArray& bytes)
{
    const QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (!doc.isObject())
        return std::nullopt;

    const QJsonObject obj = doc.object();

    if (!obj.contains("id") ||
        !obj.contains("type") ||
        !obj.contains("createdBy") ||
        !obj.contains("title") ||
        !obj.contains("createdAt")) {
        return std::nullopt;
    }

    return ChatInfo(
        QUuid(obj["id"].toString()),
        obj["type"].toString(),
        QUuid(obj["createdBy"].toString()),
        obj["title"].toString(),
        QDateTime::fromString(obj["createdAt"].toString(), Qt::ISODate)
        );
}

}
