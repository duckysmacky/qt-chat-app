#include "ChatKeyStore.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>

#include <utility>

#include "app_files.h"
#include "crypto.h"

namespace {

constexpr auto keystoreFileName = "keystore.json";
constexpr auto versionKey = "version";
constexpr auto usersKey = "users";
constexpr auto saltKey = "salt";
constexpr auto chatKeysKey = "chatKeys";
constexpr auto keyKey = "key";
constexpr auto checksumKey = "checksum";
constexpr int legacyRawVersion = 1;
constexpr int currentVersion = 2;
constexpr qsizetype saltSize = 16;

QString uuidKey(const QUuid& uuid)
{
    return uuid.toString(QUuid::WithoutBraces);
}

QByteArray base64Decode(const QJsonValue& value)
{
    return QByteArray::fromBase64(value.toString().toLatin1());
}

QString base64Encode(const QByteArray& bytes)
{
    return QString::fromLatin1(bytes.toBase64());
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

bool ChatKeyStore::unlockUser(const QUuid& userId, const QString& passwordHash)
{
    if (userId.isNull() || passwordHash.isEmpty())
        return false;

    QByteArray salt = m_userSalts.value(userId);
    if (salt.isEmpty())
    {
        salt = shared::crypto::generateRandomBytes(saltSize);
        m_userSalts.insert(userId, salt);
    }

    const QByteArray passwordKey = userPasswordKey(userId, passwordHash, salt);
    QHash<QUuid, QByteArray> decryptedKeys = m_userChatKeys.value(userId);

    const auto encryptedIt = m_encryptedUserChatKeys.constFind(userId);
    if (encryptedIt != m_encryptedUserChatKeys.constEnd())
    {
        for (auto keyIt = encryptedIt->constBegin(); keyIt != encryptedIt->constEnd(); ++keyIt)
        {
            const QUuid chatId = keyIt.key();
            const QByteArray decrypted = shared::crypto::decryptWithMasterKey(keyIt.value(), passwordKey);
            if (decrypted.size() != shared::crypto::masterKeySize())
            {
                qWarning() << "Ignoring undecryptable chat key for chat" << chatId;
                continue;
            }

            const QString storedChecksum = m_chatKeyChecksums.value(userId).value(chatId);
            if (!storedChecksum.isEmpty() && storedChecksum != checksum(userId, chatId, decrypted, passwordKey))
            {
                qWarning() << "Ignoring chat key with invalid checksum for chat" << chatId;
                continue;
            }

            decryptedKeys.insert(chatId, decrypted);
        }
    }

    m_userPasswordKeys.insert(userId, passwordKey);
    m_userChatKeys.insert(userId, decryptedKeys);

    for (auto keyIt = decryptedKeys.constBegin(); keyIt != decryptedKeys.constEnd(); ++keyIt)
    {
        m_encryptedUserChatKeys[userId].insert(
            keyIt.key(),
            shared::crypto::encryptWithMasterKey(keyIt.value(), passwordKey)
        );
        m_chatKeyChecksums[userId].insert(keyIt.key(), checksum(userId, keyIt.key(), keyIt.value(), passwordKey));
    }

    save();
    return true;
}

void ChatKeyStore::lockUser(const QUuid& userId)
{
    m_userChatKeys.remove(userId);
    m_userPasswordKeys.remove(userId);
}

bool ChatKeyStore::isUnlocked(const QUuid& userId) const
{
    return m_userPasswordKeys.contains(userId);
}

std::optional<QByteArray> ChatKeyStore::chatKey(const QUuid& userId, const QUuid& chatId) const
{
    if (userId.isNull() || chatId.isNull() || !isUnlocked(userId))
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

    if (!isUnlocked(userId))
    {
        qWarning() << "Cannot store chat key for locked user" << userId;
        return;
    }

    const auto existingKey = chatKey(userId, chatId);
    if (existingKey.has_value() && existingKey.value() == key)
        return;

    const QByteArray passwordKey = m_userPasswordKeys.value(userId);
    m_userChatKeys[userId].insert(chatId, key);
    m_encryptedUserChatKeys[userId].insert(chatId, shared::crypto::encryptWithMasterKey(key, passwordKey));
    m_chatKeyChecksums[userId].insert(chatId, checksum(userId, chatId, key, passwordKey));
    save();
}

void ChatKeyStore::removeChatKey(const QUuid& userId, const QUuid& chatId)
{
    bool changed = false;

    auto decryptedIt = m_userChatKeys.find(userId);
    if (decryptedIt != m_userChatKeys.end())
    {
        changed = decryptedIt->remove(chatId) || changed;
        if (decryptedIt->isEmpty())
            m_userChatKeys.erase(decryptedIt);
    }

    auto encryptedIt = m_encryptedUserChatKeys.find(userId);
    if (encryptedIt != m_encryptedUserChatKeys.end())
    {
        changed = encryptedIt->remove(chatId) || changed;
        if (encryptedIt->isEmpty())
            m_encryptedUserChatKeys.erase(encryptedIt);
    }

    auto checksumIt = m_chatKeyChecksums.find(userId);
    if (checksumIt != m_chatKeyChecksums.end())
    {
        checksumIt->remove(chatId);
        if (checksumIt->isEmpty())
            m_chatKeyChecksums.erase(checksumIt);
    }

    if (changed)
        save();
}

void ChatKeyStore::clearUser(const QUuid& userId)
{
    const bool changed =
        m_userChatKeys.remove(userId) ||
        m_encryptedUserChatKeys.remove(userId) ||
        m_chatKeyChecksums.remove(userId) ||
        m_userSalts.remove(userId);

    m_userPasswordKeys.remove(userId);

    if (changed)
        save();
}

void ChatKeyStore::load()
{
    m_userChatKeys.clear();
    m_encryptedUserChatKeys.clear();
    m_chatKeyChecksums.clear();
    m_userSalts.clear();

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
    if (!root[usersKey].isObject())
    {
        qWarning() << "Ignoring chat key store without users object";
        return;
    }

    const int version = root[versionKey].toInt();
    if (version == legacyRawVersion)
    {
        loadVersion1(root[usersKey].toObject());
        return;
    }

    if (version == currentVersion)
    {
        loadVersion2(root[usersKey].toObject());
        return;
    }

    qWarning() << "Ignoring unsupported chat key store version" << version;
}

void ChatKeyStore::loadVersion1(const QJsonObject& users)
{
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
            const QByteArray key = base64Decode(keyIt.value());
            if (!chatId.isNull() && !key.isEmpty())
                chatKeys.insert(chatId, key);
        }

        if (!chatKeys.isEmpty())
            m_userChatKeys.insert(userId, std::move(chatKeys));
    }
}

void ChatKeyStore::loadVersion2(const QJsonObject& users)
{
    for (auto userIt = users.constBegin(); userIt != users.constEnd(); ++userIt)
    {
        const QUuid userId(userIt.key());
        if (userId.isNull() || !userIt->isObject())
            continue;

        const QJsonObject userObj = userIt->toObject();
        const QByteArray salt = base64Decode(userObj[saltKey]);
        if (salt.isEmpty() || !userObj[chatKeysKey].isObject())
            continue;

        m_userSalts.insert(userId, salt);

        const QJsonObject keysObj = userObj[chatKeysKey].toObject();
        for (auto keyIt = keysObj.constBegin(); keyIt != keysObj.constEnd(); ++keyIt)
        {
            const QUuid chatId(keyIt.key());
            if (chatId.isNull() || !keyIt->isObject())
                continue;

            const QJsonObject keyObj = keyIt->toObject();
            const QByteArray encryptedKey = base64Decode(keyObj[keyKey]);
            const QString storedChecksum = keyObj[checksumKey].toString();
            if (encryptedKey.isEmpty())
                continue;

            m_encryptedUserChatKeys[userId].insert(chatId, encryptedKey);
            if (!storedChecksum.isEmpty())
                m_chatKeyChecksums[userId].insert(chatId, storedChecksum);
        }
    }
}

void ChatKeyStore::save() const
{
    QSet<QUuid> userIds;
    for (auto it = m_encryptedUserChatKeys.constBegin(); it != m_encryptedUserChatKeys.constEnd(); ++it)
        userIds.insert(it.key());
    for (auto it = m_userChatKeys.constBegin(); it != m_userChatKeys.constEnd(); ++it)
        userIds.insert(it.key());
    for (auto it = m_userSalts.constBegin(); it != m_userSalts.constEnd(); ++it)
        userIds.insert(it.key());

    QJsonObject users;
    for (const QUuid& userId : userIds)
    {
        const QByteArray salt = m_userSalts.value(userId);
        if (salt.isEmpty())
            continue;

        QJsonObject chatKeys;
        const QHash<QUuid, QByteArray> encryptedKeys = m_encryptedUserChatKeys.value(userId);
        const QHash<QUuid, QString> checksums = m_chatKeyChecksums.value(userId);
        for (auto keyIt = encryptedKeys.constBegin(); keyIt != encryptedKeys.constEnd(); ++keyIt)
        {
            QJsonObject keyObj;
            keyObj[keyKey] = base64Encode(keyIt.value());
            keyObj[checksumKey] = checksums.value(keyIt.key());
            chatKeys[uuidKey(keyIt.key())] = keyObj;
        }

        QJsonObject userObj;
        userObj[saltKey] = base64Encode(salt);
        userObj[chatKeysKey] = chatKeys;
        users[uuidKey(userId)] = userObj;
    }

    QJsonObject root;
    root[versionKey] = currentVersion;
    root[usersKey] = users;

    QString error;
    const QByteArray bytes = QJsonDocument(root).toJson(QJsonDocument::Indented);
    if (!appFiles::writeFile(appFiles::Location::Data, keystoreFileName, bytes, &error))
        qWarning() << "Unable to save chat key store:" << error;
}

QByteArray ChatKeyStore::userPasswordKey(const QUuid& userId, const QString& passwordHash, const QByteArray& salt) const
{
    QByteArray bytes;
    bytes.append(passwordHash.toUtf8());
    bytes.append(userId.toRfc4122());
    bytes.append(salt);

    return QCryptographicHash::hash(bytes, QCryptographicHash::Sha256);
}

QString ChatKeyStore::checksum(
    const QUuid& userId,
    const QUuid& chatId,
    const QByteArray& key,
    const QByteArray& passwordKey
) const
{
    QByteArray bytes;
    bytes.append("qt-chat-app-chat-key");
    bytes.append(userId.toRfc4122());
    bytes.append(chatId.toRfc4122());
    bytes.append(passwordKey);
    bytes.append(key);

    return QString::fromLatin1(QCryptographicHash::hash(bytes, QCryptographicHash::Sha256).toHex());
}
