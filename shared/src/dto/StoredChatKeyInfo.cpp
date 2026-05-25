#include "StoredChatKeyInfo.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <utility>

namespace shared {

namespace {

constexpr auto userIdKey = "userId";
constexpr auto chatIdKey = "chatId";
constexpr auto saltKey = "salt";
constexpr auto encryptedKeyKey = "encryptedKey";
constexpr auto checksumKey = "checksum";
constexpr auto keysKey = "keys";

QJsonObject toJson(const StoredChatKeyInfo& info)
{
    QJsonObject obj;
    obj[userIdKey] = info.userId().toString(QUuid::WithoutBraces);
    obj[chatIdKey] = info.chatId().toString(QUuid::WithoutBraces);
    obj[saltKey] = QString::fromLatin1(info.salt().toBase64());
    obj[encryptedKeyKey] = QString::fromLatin1(info.encryptedKey().toBase64());
    obj[checksumKey] = info.checksum();
    return obj;
}

std::optional<StoredChatKeyInfo> fromJson(const QJsonObject& obj)
{
    const QUuid userId(obj[userIdKey].toString());
    const QUuid chatId(obj[chatIdKey].toString());
    const QByteArray salt = QByteArray::fromBase64(obj[saltKey].toString().toLatin1());
    const QByteArray encryptedKey = QByteArray::fromBase64(obj[encryptedKeyKey].toString().toLatin1());
    const QString checksum = obj[checksumKey].toString();

    if (userId.isNull() || chatId.isNull() || salt.isEmpty() || encryptedKey.isEmpty() || checksum.isEmpty())
        return std::nullopt;

    return StoredChatKeyInfo(userId, chatId, salt, encryptedKey, checksum);
}

} // namespace

StoredChatKeyInfo::StoredChatKeyInfo(QUuid userId, QUuid chatId, QByteArray salt, QByteArray encryptedKey, QString checksum)
    : m_userId(std::move(userId)),
      m_chatId(std::move(chatId)),
      m_salt(std::move(salt)),
      m_encryptedKey(std::move(encryptedKey)),
      m_checksum(std::move(checksum))
{
}

QByteArray StoredChatKeyInfo::serialize() const
{
    return QJsonDocument(toJson(*this)).toJson(QJsonDocument::Compact);
}

std::optional<StoredChatKeyInfo> StoredChatKeyInfo::deserialize(const QByteArray& bytes)
{
    const QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (!doc.isObject())
        return std::nullopt;

    return fromJson(doc.object());
}

ChatKeysInfo::ChatKeysInfo(QList<StoredChatKeyInfo> keys)
    : m_keys(std::move(keys))
{
}

QByteArray ChatKeysInfo::serialize() const
{
    QJsonArray keys;
    for (const auto& key : m_keys)
        keys.append(toJson(key));

    QJsonObject obj;
    obj[keysKey] = keys;
    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

std::optional<ChatKeysInfo> ChatKeysInfo::deserialize(const QByteArray& bytes)
{
    const QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (!doc.isObject())
        return std::nullopt;

    const QJsonObject obj = doc.object();
    if (!obj[keysKey].isArray())
        return std::nullopt;

    QList<StoredChatKeyInfo> keys;
    for (const auto& value : obj[keysKey].toArray())
    {
        if (!value.isObject())
            continue;

        const auto key = fromJson(value.toObject());
        if (key.has_value())
            keys.append(key.value());
    }

    return ChatKeysInfo(std::move(keys));
}

} // namespace shared
