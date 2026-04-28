#pragma once

#include <QByteArray>
#include <QList>

#include <optional>

#include "ChatInfo.h"

namespace shared {

class ChatsInfo
{
public:
    ChatsInfo();
    explicit ChatsInfo(QList<ChatInfo> chats);

    const QList<ChatInfo>& chats() const { return m_chats; }

    QByteArray serialize() const;
    static std::optional<ChatsInfo> deserialize(const QByteArray& bytes);

private:
    QList<ChatInfo> m_chats;
};

}
