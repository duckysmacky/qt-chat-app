#include "UserConnection.h"

#include <QByteArray>
#include <QDebug>

#include "util.h"

UserConnection::UserConnection(const QUuid& sessionId, QTcpSocket* socket)
    : m_sessionId(sessionId),
    m_socket(socket)
{
}

void UserConnection::authorize(const QUuid& userId)
{
    m_userId = userId;
}

void UserConnection::logout()
{
    m_userId.reset();
}

bool UserConnection::matchesSocket(const QTcpSocket* socket) const
{
    return m_socket == socket;
}

bool UserConnection::writePacket(const shared::Packet& packet) const
{
    if (!m_socket) {
        qWarning() << "Cannot send packet: socket is null";
        return false;
    }

    QByteArray payload;
    payload.append(packet.serialize());
    payload.append(shared::util::DELIMITER);

    if (m_socket->write(payload) == -1) {
        qCritical() << "Error writing packet:" << m_socket->errorString();
        return false;
    }

    return true;
}
