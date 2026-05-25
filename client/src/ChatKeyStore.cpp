#include "ChatKeyStore.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#include <utility>

#include "app_files.h"

namespace {

constexpr auto keystoreFileName = "keystore.json";
constexpr auto versionKey = "version";
constexpr auto usersKey = "users";
constexpr auto chatKeysKey = "chatKeys";
constexpr int currentVersion = 1;

QString uuidKey(const QUuid& uuid)
{
    return uuid.toString(QUuid::WithoutBraces);
}

} // namespace

ChatKeyStore& ChatKeyStore::instance()
{
    static ChatKeyStore store;
    return store;
}

ChatKeyStore::ChatKeyStore()
{
    load();
}

std::optional<QByteArray> ChatKeyStore::chatKey(const QUuid& userId, const QUuid& chatId) const
{
    if (userId.isNull() || chatId.isNull())
        return std::nullopt;

    const auto userIt = m_userChatKeys.constFind(userId);
    if (userIt == m_userChatKeys.constEnd())
        return std::nullopt;

    const auto chatIt = userIt->constFind(chatId);
    if (chatIt == userIt->constEnd())
        return std::nullopt;

    return chatIt.value();
}

void ChatKeyStore::setChatKey(const QUuid& userId, const QUuid& chatId, QByteArray key)
{
    if (userId.isNull() || chatId.isNull() || key.isEmpty())
        return;

    const auto existingKey = chatKey(userId, chatId);
    if (existingKey.has_value() && existingKey.value() == key)
        return;

    m_userChatKeys[userId].insert(chatId, std::move(key));
    save();
}

void ChatKeyStore::removeChatKey(const QUuid& userId, const QUuid& chatId)
{
    auto userIt = m_userChatKeys.find(userId);
    if (userIt == m_userChatKeys.end())
        return;

    userIt->remove(chatId);
    if (userIt->isEmpty())
        m_userChatKeys.erase(userIt);

    save();
}

void ChatKeyStore::clearUser(const QUuid& userId)
{
    if (m_userChatKeys.remove(userId))
        save();
}

void ChatKeyStore::load()
{
    m_userChatKeys.clear();

    QString error;
    const auto bytes = appFiles::readFile(appFiles::Location::Data, keystoreFileName, &error);
    if (!bytes.has_value())
    {
        if (appFiles::exists(appFiles::Location::Data, keystoreFileName))
            qWarning() << "Unable to read chat key store:" << error;

        return;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(bytes.value());
    if (!doc.isObject())
    {
        qWarning() << "Ignoring invalid chat key store JSON";
        return;
    }

    const QJsonObject root = doc.object();
    if (root[versionKey].toInt() != currentVersion || !root[usersKey].isObject())
    {
        qWarning() << "Ignoring unsupported chat key store format";
        return;
    }

    const QJsonObject users = root[usersKey].toObject();
    for (auto userIt = users.constBegin(); userIt != users.constEnd(); ++userIt)
    {
        const QUuid userId(userIt.key());
        if (userId.isNull() || !userIt->isObject())
            continue;

        const QJsonObject userObj = userIt->toObject();
        if (!userObj[chatKeysKey].isObject())
            continue;

        QHash<QUuid, QByteArray> chatKeys;
        const QJsonObject keysObj = userObj[chatKeysKey].toObject();
        for (auto keyIt = keysObj.constBegin(); keyIt != keysObj.constEnd(); ++keyIt)
        {
            const QUuid chatId(keyIt.key());
            const QByteArray key = QByteArray::fromBase64(keyIt->toString().toLatin1());
            if (!chatId.isNull() && !key.isEmpty())
                chatKeys.insert(chatId, key);
        }

        if (!chatKeys.isEmpty())
            m_userChatKeys.insert(userId, std::move(chatKeys));
    }
}

void ChatKeyStore::save() const
{
    QJsonObject users;
    for (auto userIt = m_userChatKeys.constBegin(); userIt != m_userChatKeys.constEnd(); ++userIt)
    {
        QJsonObject chatKeys;
        const QHash<QUuid, QByteArray>& userKeys = userIt.value();
        for (auto keyIt = userKeys.constBegin(); keyIt != userKeys.constEnd(); ++keyIt)
            chatKeys[uuidKey(keyIt.key())] = QString::fromLatin1(keyIt.value().toBase64());

        QJsonObject userObj;
        userObj[chatKeysKey] = chatKeys;
        users[uuidKey(userIt.key())] = userObj;
    }

    QJsonObject root;
    root[versionKey] = currentVersion;
    root[usersKey] = users;

    QString error;
    const QByteArray bytes = QJsonDocument(root).toJson(QJsonDocument::Indented);
    if (!appFiles::writeFile(appFiles::Location::Data, keystoreFileName, bytes, &error))
        qWarning() << "Unable to save chat key store:" << error;
}
