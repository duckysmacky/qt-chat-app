#pragma once

#include <QByteArray>
#include <QHash>
#include <QUuid>

#include <optional>

class ChatKeyStore
{
private:
    QHash<QUuid, QHash<QUuid, QByteArray>> m_userChatKeys;

public:
    static ChatKeyStore& instance();

    ChatKeyStore(const ChatKeyStore&) = delete;
    ChatKeyStore& operator=(const ChatKeyStore&) = delete;
    ChatKeyStore(ChatKeyStore&&) = delete;
    ChatKeyStore& operator=(ChatKeyStore&&) = delete;

    std::optional<QByteArray> chatKey(const QUuid& userId, const QUuid& chatId) const;
    void setChatKey(const QUuid& userId, const QUuid& chatId, QByteArray key);
    void removeChatKey(const QUuid& userId, const QUuid& chatId);
    void clearUser(const QUuid& userId);

private:
    ChatKeyStore();

    void load();
    void save() const;
};
