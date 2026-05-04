#include "ChatInfo.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <utility>

namespace shared {

ChatInfo::ChatInfo() = default;

ChatInfo::ChatInfo(QUuid id, QString type, QUuid createdBy, QString title, QDateTime createdAt)
    : ChatInfo(std::move(id), std::move(type), std::move(createdBy), std::move(title), std::move(createdAt), {})
{
}

ChatInfo::ChatInfo(QUuid id, QString type, QUuid createdBy, QString title, QDateTime createdAt, QList<QUuid> memberIds)
    : m_id(std::move(id)),
    m_type(std::move(type)),
    m_createdBy(std::move(createdBy)),
    m_title(std::move(title)),
    m_createdAt(std::move(createdAt)),
    m_memberIds(std::move(memberIds))
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

    QJsonArray memberIds;
    for (const QUuid& memberId : m_memberIds)
        memberIds.append(memberId.toString(QUuid::WithoutBraces));
    obj["memberIds"] = memberIds;

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
        !obj.contains("createdAt") ||
        !obj.contains("memberIds") ||
        !obj["memberIds"].isArray()) {
        return std::nullopt;
    }

    QList<QUuid> memberIds;
    for (const auto& value : obj["memberIds"].toArray()) {
        if (!value.isString())
            return std::nullopt;

        const QUuid memberId(value.toString());
        if (memberId.isNull())
            return std::nullopt;

        memberIds.append(memberId);
    }

    return ChatInfo(
        QUuid(obj["id"].toString()),
        obj["type"].toString(),
        QUuid(obj["createdBy"].toString()),
        obj["title"].toString(),
        QDateTime::fromString(obj["createdAt"].toString(), Qt::ISODate),
        std::move(memberIds)
        );
}

}
