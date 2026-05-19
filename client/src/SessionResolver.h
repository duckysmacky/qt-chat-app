#pragma once

#include <QHash>
#include <QUuid>

class SessionResolver
{
private:
    QUuid m_serverSessionId;
    QHash<QUuid, QUuid> m_userSessionIds;

public:
    static SessionResolver& instance();

    SessionResolver(const SessionResolver& other) = delete;
    SessionResolver& operator=(const SessionResolver& other) = delete;
    SessionResolver(SessionResolver&& other) = delete;
    SessionResolver& operator=(SessionResolver&& other) = delete;

    const QUuid& serverSessionId() const { return m_serverSessionId; }
    void setServerSessionId(const QUuid& sessionId);

    QUuid userSessionId(const QUuid& userId) const;
    void setUserSessionId(const QUuid& userId, const QUuid& sessionId);

private:
    SessionResolver() = default;
};