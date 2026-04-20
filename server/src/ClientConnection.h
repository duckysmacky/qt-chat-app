#pragma once

#include <QTcpSocket>
#include <QUuid>

#include <optional>

#include "Packet.h"

/**
 * @class ClientConnection
 * @brief Represents a connected client session with authorization state.
 */
class ClientConnection
{
private:
    QUuid m_sessionId;                      ///< Unique session identifier for this connection
    QTcpSocket* m_socket;                   ///< Pointer to the TCP socket associated with this client
    std::optional<QUuid> m_userId;          ///< Authenticated user ID (empty if not authorized)
    bool m_isAuthorized;                    ///< Flag indicating whether the client is authorized

public:
    /**
     * @brief Constructs a new ClientConnection object.
     * @param sessionId Unique session identifier.
     * @param socket Pointer to the client's TCP socket.
     */
    ClientConnection(const QUuid& sessionId, QTcpSocket* socket);

    /**
     * @brief Authorizes the client with a specific user ID.
     * @param userId The UUID of the authenticated user.
     */
    void authorize(const QUuid& userId);

    /**
     * @brief Logs out the client, clearing authorization state.
     */
    void logout();

    /**
     * @brief Checks whether this connection uses the given socket.
     * @param socket Pointer to the socket to compare.
     * @return true if the socket matches, false otherwise.
     */
    bool matchesSocket(const QTcpSocket* socket) const;

    /**
     * @brief Sends a packet to the client.
     * @param packet The packet to send.
     * @return true if the packet was sent successfully, false otherwise.
     */
    bool sendPacket(const shared::Packet& packet) const;

    /// @brief Returns the session ID of this connection.
    /// @return Constant reference to the session UUID.
    const QUuid& sessionId() const { return m_sessionId; }

    /// @brief Returns the authenticated user ID (if any).
    /// @return Constant reference to an optional containing the user UUID.
    const std::optional<QUuid>& userId() const { return m_userId; }

    /// @brief Checks whether the client is currently authorized.
    /// @return true if authorized, false otherwise.
    bool isAuthorized() const { return m_isAuthorized; }
};