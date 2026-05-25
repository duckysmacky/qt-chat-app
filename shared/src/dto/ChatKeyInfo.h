#pragma once

#include <QByteArray>
#include <QUuid>

#include <optional>

namespace shared {

class ChatKeyInfo
{
private:
    QUuid m_chatId;
    QByteArray m_masterKey;

public:
    ChatKeyInfo(QUuid chatId, QByteArray masterKey);

    const QUuid& chatId() const { return m_chatId; }
    const QByteArray& masterKey() const { return m_masterKey; }

    QByteArray serialize() const;
    static std::optional<ChatKeyInfo> deserialize(const QByteArray& bytes);
};

} // namespace shared
