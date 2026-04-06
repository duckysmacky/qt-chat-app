#include "ClientConnection.h"

#include <QByteArray>
#include <QDebug>

#include "util.h"

ClientConnection::ClientConnection(const QUuid& sessionId, QTcpSocket* socket)
    : m_sessionId(sessionId),
      m_socket(socket),
      m_isAuthorized(false)
{
}

void ClientConnection::authorize(const QUuid& userId)
{
    m_isAuthorized = true;
    m_userId = userId;
}

void ClientConnection::logout()
{
    m_isAuthorized = false;
    m_userId.reset();
}

bool ClientConnection::sendPacket(const shared::Packet& packet) const
{
    if (!m_socket)
    {
        qWarning() << "Cannot send packet: socket is null";
        return false;
    }

    QByteArray payload = shared::util::encapsulate(packet);

    if (m_socket->write(payload) == -1)
    {
        qCritical() << "Error writing packet:" << m_socket->errorString();
        return false;
    }

    return true;
}

bool ClientConnection::matchesSocket(const QTcpSocket* socket) const
{
    return socket != nullptr && socket != m_socket;
}
