#include "CreateChatInfo.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <utility>

namespace shared {

ChatCreateInfo::ChatCreateInfo(QList<QUuid> memberIds)
    : m_memberIds(std::move(memberIds))
{
}

QByteArray ChatCreateInfo::serialize() const
{
    QJsonObject obj;

    QJsonArray memberIds;
    for (const QUuid& memberId : m_memberIds)
        memberIds.append(memberId.toString(QUuid::WithoutBraces));
    obj["memberIds"] = memberIds;

    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

std::optional<ChatCreateInfo> ChatCreateInfo::deserialize(const QByteArray& bytes)
{
    const QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (!doc.isObject())
        return std::nullopt;

    const QJsonObject obj = doc.object();

    if (!obj.contains("memberIds") || !obj["memberIds"].isArray())
        return std::nullopt;

    QList<QUuid> memberIds;
    for (const auto& value : obj["memberIds"].toArray()) {
        if (!value.isString())
            continue;

        const QUuid memberId(value.toString());
        if (!memberId.isNull())
            memberIds.append(memberId);
    }

    return ChatCreateInfo(std::move(memberIds));
}

}
