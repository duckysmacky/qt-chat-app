#pragma once

#include <QTcpSocket>
#include <QUuid>

#include <optional>

#include "Packet.h"

/**
 * @class ClientConnection
 * @brief Represents a single client connection over TCP.
 *
 * Manages session state, authorization status, and packet transmission
 * for a connected client.
 */
class ClientConnection
{
private:
    QUuid m_sessionId;                 ///< Unique session identifier
    QTcpSocket* m_socket;              ///< Associated TCP socket
    std::optional<QUuid> m_userId;     ///< Authorized user ID (if logged in)
    bool m_isAuthorized;               ///< Authorization state flag

public:
    /**
     * @brief Constructs a ClientConnection instance.
     *
     * @param sessionId Unique session identifier.
     * @param socket TCP socket associated with the client.
     */
    ClientConnection(const QUuid& sessionId, QTcpSocket* socket);

    /**
     * @brief Authorizes the connection for a specific user.
     *
     * @param userId Identifier of the authenticated user.
     */
    void authorize(const QUuid& userId);

    /**
     * @brief Logs out the current user.
     *
     * Clears authorization state and user identifier.
     */
    void logout();

    /**
     * @brief Checks if the given socket belongs to this connection.
     *
     * @param socket Socket to compare.
     * @return True if sockets match, false otherwise.
     */
    bool matchesSocket(const QTcpSocket* socket) const;

    /**
     * @brief Sends a packet to the client.
     *
     * Serializes and transmits the packet through the TCP socket.
     *
     * @param packet Packet to send.
     * @return True if sending succeeded, false otherwise.
     */
    bool sendPacket(const shared::Packet& packet) const;

    /// @return Session identifier.
    const QUuid& sessionId() const { return m_sessionId; }

    /// @return Optional user ID if authorized.
    const std::optional<QUuid>& userId() const { return m_userId; }

    /// @return True if the client is authorized.
    bool isAuthorized() const { return m_isAuthorized; }
};