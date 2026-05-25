#include "SessionResolver.h"

SessionResolver& SessionResolver::instance()
{
    static SessionResolver resolver;
    return resolver;
}

void SessionResolver::setServerSessionId(const QUuid& sessionId)
{
    if (!sessionId.isNull())
        m_serverSessionId = sessionId;
}

QUuid SessionResolver::userSessionId(const QUuid& userId) const
{
    return m_userSessionIds.value(userId);
}

QUuid SessionResolver::sessionUserId(const QUuid& sessionId) const
{
    return m_sessionUserIds.value(sessionId);
}

void SessionResolver::setUserSessionId(const QUuid& userId, const QUuid& sessionId)
{
    if (userId.isNull())
        return;

    const QUuid previousSessionId = m_userSessionIds.value(userId);
    if (!previousSessionId.isNull())
        m_sessionUserIds.remove(previousSessionId);

    if (sessionId.isNull())
    {
        m_userSessionIds.remove(userId);
    }
    else
    {
        m_userSessionIds.insert(userId, sessionId);
        m_sessionUserIds.insert(sessionId, userId);
    }
}
