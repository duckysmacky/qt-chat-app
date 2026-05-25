#pragma once

#include <QByteArray>
#include <QHash>
#include <QJsonObject>
#include <QString>
#include <QUuid>

#include <optional>

class ChatKeyStore
{
private:
    QHash<QUuid, QHash<QUuid, QByteArray>> m_userChatKeys;
    QHash<QUuid, QHash<QUuid, QByteArray>> m_encryptedUserChatKeys;
    QHash<QUuid, QHash<QUuid, QString>> m_chatKeyChecksums;
    QHash<QUuid, QByteArray> m_userSalts;
    QHash<QUuid, QByteArray> m_userPasswordKeys;

public:
    static ChatKeyStore& instance();

    ChatKeyStore(const ChatKeyStore&) = delete;
    ChatKeyStore& operator=(const ChatKeyStore&) = delete;
    ChatKeyStore(ChatKeyStore&&) = delete;
    ChatKeyStore& operator=(ChatKeyStore&&) = delete;

    bool unlockUser(const QUuid& userId, const QString& passwordHash);
    void lockUser(const QUuid& userId);
    bool isUnlocked(const QUuid& userId) const;

    std::optional<QByteArray> chatKey(const QUuid& userId, const QUuid& chatId) const;
    void setChatKey(const QUuid& userId, const QUuid& chatId, QByteArray key);
    void removeChatKey(const QUuid& userId, const QUuid& chatId);
    void clearUser(const QUuid& userId);

private:
    ChatKeyStore();

    void load();
    void loadVersion1(const QJsonObject& users);
    void loadVersion2(const QJsonObject& users);
    void save() const;
    QByteArray userPasswordKey(const QUuid& userId, const QString& passwordHash, const QByteArray& salt) const;
    QString checksum(const QUuid& userId, const QUuid& chatId, const QByteArray& key, const QByteArray& passwordKey) const;
};
