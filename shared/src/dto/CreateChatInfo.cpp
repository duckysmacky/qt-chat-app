#include "CreateChatInfo.h"

#include <QJsonDocument>
#include <QJsonObject>

#include <utility>

namespace shared {

ChatCreateInfo::ChatCreateInfo() = default;

ChatCreateInfo::ChatCreateInfo(QString type, QString title)
    : m_type(std::move(type)),
    m_title(std::move(title))
{
}

QByteArray ChatCreateInfo::serialize() const
{
    QJsonObject obj;
    obj["type"] = m_type;
    obj["title"] = m_title;

    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

std::optional<ChatCreateInfo> ChatCreateInfo::deserialize(const QByteArray& bytes)
{
    const QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (!doc.isObject())
        return std::nullopt;

    const QJsonObject obj = doc.object();

    if (!obj.contains("type") || !obj.contains("title"))
        return std::nullopt;

    return ChatCreateInfo(
        obj["type"].toString(),
        obj["title"].toString()
        );
}

}
