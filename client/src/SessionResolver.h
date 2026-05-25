#pragma once

#include <QHash>
#include <QUuid>

class SessionResolver
{
private:
    QUuid m_serverSessionId;
    QHash<QUuid, QUuid> m_userSessionIds;
    QHash<QUuid, QUuid> m_sessionUserIds;

public:
    static SessionResolver& instance();

    SessionResolver(const SessionResolver& other) = delete;
    SessionResolver& operator=(const SessionResolver& other) = delete;
    SessionResolver(SessionResolver&& other) = delete;
    SessionResolver& operator=(SessionResolver&& other) = delete;

    const QUuid& serverSessionId() const { return m_serverSessionId; }
    void setServerSessionId(const QUuid& sessionId);

    QUuid userSessionId(const QUuid& userId) const;
    QUuid sessionUserId(const QUuid& sessionId) const;
    void setUserSessionId(const QUuid& userId, const QUuid& sessionId);

private:
    SessionResolver() = default;
};
