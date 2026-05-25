#include "ChatKeyInfo.h"

#include <QJsonDocument>
#include <QJsonObject>

#include <utility>

namespace shared {

namespace {

constexpr auto chatIdKey = "chatId";
constexpr auto masterKeyKey = "masterKey";

} // namespace

ChatKeyInfo::ChatKeyInfo(QUuid chatId, QByteArray masterKey)
    : m_chatId(std::move(chatId)),
      m_masterKey(std::move(masterKey))
{
}

QByteArray ChatKeyInfo::serialize() const
{
    QJsonObject obj;
    obj[chatIdKey] = m_chatId.toString(QUuid::WithoutBraces);
    obj[masterKeyKey] = QString::fromLatin1(m_masterKey.toBase64());

    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

std::optional<ChatKeyInfo> ChatKeyInfo::deserialize(const QByteArray& bytes)
{
    const QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (!doc.isObject())
        return std::nullopt;

    const QJsonObject obj = doc.object();
    if (!obj.contains(chatIdKey) || !obj.contains(masterKeyKey))
        return std::nullopt;

    const QUuid chatId(obj[chatIdKey].toString());
    const QByteArray masterKey = QByteArray::fromBase64(obj[masterKeyKey].toString().toLatin1());
    if (chatId.isNull() || masterKey.isEmpty())
        return std::nullopt;

    return ChatKeyInfo(chatId, masterKey);
}

} // namespace shared
