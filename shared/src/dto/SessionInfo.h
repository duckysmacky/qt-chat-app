#pragma once

#include <QByteArray>
#include <QUuid>

#include <optional>

namespace shared {

class SessionInfo
{
private:
    QUuid m_userId;
    QUuid m_sessionId;

public:
    SessionInfo(QUuid userId, QUuid sessionId);

    const QUuid& userId() const { return m_userId; }
    const QUuid& sessionId() const { return m_sessionId; }

    QByteArray serialize() const;
    static std::optional<SessionInfo> deserialize(const QByteArray& bytes);
};

} // namespace shared
