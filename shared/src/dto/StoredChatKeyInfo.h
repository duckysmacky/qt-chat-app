#pragma once

#include <QByteArray>
#include <QList>
#include <QString>
#include <QUuid>

#include <optional>

namespace shared {

class StoredChatKeyInfo
{
private:
    QUuid m_userId;
    QUuid m_chatId;
    QByteArray m_salt;
    QByteArray m_encryptedKey;
    QString m_checksum;

public:
    StoredChatKeyInfo(QUuid userId, QUuid chatId, QByteArray salt, QByteArray encryptedKey, QString checksum);

    const QUuid& userId() const { return m_userId; }
    const QUuid& chatId() const { return m_chatId; }
    const QByteArray& salt() const { return m_salt; }
    const QByteArray& encryptedKey() const { return m_encryptedKey; }
    const QString& checksum() const { return m_checksum; }

    QByteArray serialize() const;
    static std::optional<StoredChatKeyInfo> deserialize(const QByteArray& bytes);
};

class ChatKeysInfo
{
private:
    QList<StoredChatKeyInfo> m_keys;

public:
    explicit ChatKeysInfo(QList<StoredChatKeyInfo> keys);

    const QList<StoredChatKeyInfo>& keys() const { return m_keys; }

    QByteArray serialize() const;
    static std::optional<ChatKeysInfo> deserialize(const QByteArray& bytes);
};

} // namespace shared
