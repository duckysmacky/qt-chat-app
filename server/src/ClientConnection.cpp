#include "ClientConnection.h"

#include <QByteArray>
#include <QDebug>

#include "util.h"

/**
 * @brief Constructs a client connection instance.
 *
 * @param sessionId Unique session identifier.
 * @param socket TCP socket associated with the client.
 */
ClientConnection::ClientConnection(const QUuid& sessionId, QTcpSocket* socket)
    : m_sessionId(sessionId),
      m_socket(socket),
      m_isAuthorized(false)
{
}

/**
 * @brief Authorizes the client connection.
 *
 * Marks the connection as authorized and assigns a user ID.
 *
 * @param userId Identifier of the authorized user.
 */
void ClientConnection::authorize(const QUuid& userId)
{
    m_isAuthorized = true;
    m_userId = userId;
}

/**
 * @brief Logs out the client.
 *
 * Resets authorization state and clears the user ID.
 */
void ClientConnection::logout()
{
    m_isAuthorized = false;
    m_userId.reset();
}

/**
 * @brief Sends a packet through the TCP socket.
 *
 * Encapsulates the packet into a binary payload and writes it to the socket.
 *
 * @param packet Packet to send.
 * @return True if the packet was successfully written, false otherwise.
 */
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

/**
 * @brief Checks whether the given socket belongs to this connection.
 *
 * @param socket Socket to compare.
 * @return True if the socket matches this connection's socket.
 */
bool ClientConnection::matchesSocket(const QTcpSocket* socket) const
{
    return socket != nullptr && socket == m_socket;
}