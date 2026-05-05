#pragma once

#include <QByteArray>
#include <QList>
#include <QUuid>

#include <optional>

namespace shared {

class ChatCreateInfo
{
public:
    explicit ChatCreateInfo(QList<QUuid> memberIds);

    const QList<QUuid>& memberIds() const { return m_memberIds; }

    QByteArray serialize() const;
    static std::optional<ChatCreateInfo> deserialize(const QByteArray& bytes);

private:
    QList<QUuid> m_memberIds;
};

}
