#include "ChatsInfo.h"

#include <QJsonArray>
#include <QJsonObject>

#include <QJsonDocument>

#include <utility>

namespace shared {

ChatsInfo::ChatsInfo() = default;

ChatsInfo::ChatsInfo(QList<ChatInfo> chats)
    : m_chats(std::move(chats))
{
}

QByteArray ChatsInfo::serialize() const
{
    QJsonArray array;

    for (const auto& chat : m_chats)
    {
        const QJsonDocument chatDoc = QJsonDocument::fromJson(chat.serialize());
        if (chatDoc.isObject())
            array.append(chatDoc.object());
    }

    return QJsonDocument(array).toJson(QJsonDocument::Compact);
}

std::optional<ChatsInfo> ChatsInfo::deserialize(const QByteArray& bytes)
{
    const QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (!doc.isArray())
        return std::nullopt;

    QList<ChatInfo> chats;

    for (const auto& value : doc.array())
    {
        if (!value.isObject())
            return std::nullopt;

        const QByteArray chatBytes = QJsonDocument(value.toObject()).toJson(QJsonDocument::Compact);
        const auto chatInfo = ChatInfo::deserialize(chatBytes);

        if (!chatInfo.has_value())
            return std::nullopt;

        chats.append(chatInfo.value());
    }

    return ChatsInfo(std::move(chats));
}

}
