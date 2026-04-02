#include "UserConnection.h"

UserConnection::UserConnection() = default;

UserConnection::UserConnection(const QUuid& sessionId, QTcpSocket* socket)
    : m_sessionId(sessionId),
    m_socket(socket)
{
}

const QUuid& UserConnection::sessionId() const
{
    return m_sessionId;
}

QTcpSocket* UserConnection::socket() const
{
    return m_socket;
}

const std::optional<QUuid>& UserConnection::userId() const
{
    return m_userId;
}

bool UserConnection::isAuthorized() const
{
    return m_userId.has_value();
}

void UserConnection::authorize(const QUuid& userId)
{
    m_userId = userId;
}

void UserConnection::logout()
{
    m_userId.reset();
}
