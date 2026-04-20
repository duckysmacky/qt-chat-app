#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QHash>

#include <cstdint>

#include "Packet.h"
#include "ClientConnection.h"

/**
 * @class Server
 * @brief TCP server with console input and multi-client broadcasting.
 */
class Server : public QObject
{
    Q_OBJECT

private:
    const QUuid m_uuid; ///< Server UUID
    QTcpServer* m_server; ///< TCP server
    QHash<QUuid, ClientConnection> m_clients; ///< Connected clients
    bool m_isRunning; ///< Server running state (true/false)

public:
    /**
     * @brief Returns the singleton Server instance.
     * @return Reference to the server.
     */
    static Server& instance();

    /// @brief Copy constructor is deleted (singleton pattern).
    Server(const Server& other) = delete;

    /// @brief Copy assignment operator is deleted (singleton pattern).
    Server& operator =(const Server& other) = delete;

    /// @brief Move constructor is deleted (singleton pattern).
    Server(Server&& other) = delete;

    /// @brief Move assignment operator is deleted (singleton pattern).
    Server& operator =(Server&& other) = delete;

    /**
     * @brief Destructor. Stops the server and console reader.
     */
    ~Server() override;

    /**
     * @brief Starts the TCP server on the specified port.
     * @param port Port number (default 8080)
     * @return true if the server started successfully, false otherwise.
     */
    bool start(uint16_t port = 8080);

    /**
     * @brief Stops the server and the console reader.
     */
    void stop() const;

    /**
     * @brief Sends a packet to a specific client.
     * @param target Client UUID.
     * @param packet Packet to send.
     */
    void sendPacket(const QUuid& target, const shared::Packet& packet) const;

    /// @brief Sends an error message to a specific client.
    /// @param target Client UUID.
    /// @param message Error message text.
    void sendError(const QUuid& target, QString message) const;

    /// @brief Sends a success message to a specific client.
    /// @param target Client UUID.
    /// @param message Success message text.
    void sendSuccess(const QUuid& target, QString message) const;

    /**
     * @brief Checks if the server is currently running.
     * @return true if server is running, false otherwise.
     */
    bool isRunning() const { return m_isRunning; }

public slots:
    /**
     * @brief Handles new incoming TCP connections.
     */
    void onNewConnection();

    /**
     * @brief Handles client disconnections.
     */
    void onClientDisconnected();

    /**
     * @brief Reads incoming data from clients.
     */
    void onServerRead();

private:
    /// @brief Private constructor (singleton pattern).
    /// @param parent Parent QObject (default nullptr).
    explicit Server(QObject* parent = nullptr);

    /// @brief Finds a client connection by session ID (non-const version).
    /// @param sessionId The UUID of the client session.
    /// @return An optional reference to the ClientConnection if found.
    std::optional<std::reference_wrapper<ClientConnection>> findConnection(const QUuid& sessionId);

    /// @brief Finds a client connection by session ID (const version).
    /// @param sessionId The UUID of the client session.
    /// @return An optional reference to the const ClientConnection if found.
    std::optional<std::reference_wrapper<const ClientConnection>> findConnection(const QUuid& sessionId) const;

    /// @brief Finds a client connection by socket pointer (non-const version).
    /// @param clientSocket Pointer to the client socket.
    /// @return An optional reference to the ClientConnection if found.
    std::optional<std::reference_wrapper<ClientConnection>> findConnection(const QTcpSocket* clientSocket);

    /// @brief Finds a client connection by socket pointer (const version).
    /// @param clientSocket Pointer to the client socket.
    /// @return An optional reference to the const ClientConnection if found.
    std::optional<std::reference_wrapper<const ClientConnection>> findConnection(const QTcpSocket* clientSocket) const;

    /// @brief Handles a CONNECT packet from a client.
    /// @param socket The client socket.
    /// @param packet The received packet.
    void handleConnect(QTcpSocket* socket, const shared::Packet& packet);

    /// @brief Handles a chat message packet from an authenticated client.
    /// @param connection The client connection.
    /// @param packet The received packet.
    void handleChatMessage(const ClientConnection& connection, const shared::Packet& packet) const;

    /// @brief Handles a REGISTER packet for new user registration.
    /// @param socket The client socket.
    /// @param packet The received packet.
    void handleRegister(const QTcpSocket* socket, const shared::Packet& packet);

    /// @brief Handles a LOGIN packet for user authentication.
    /// @param socket The client socket.
    /// @param packet The received packet.
    void handleLogin(const QTcpSocket* socket, const shared::Packet& packet);

    /// @brief Handles an authorized packet (requires authentication).
    /// @param packet The received packet.
    void handleAuthorizedPacket(const shared::Packet& packet) const;

    /// @brief Handles a LOGOUT packet to disconnect a client.
    /// @param socket The client socket.
    /// @param packet The received packet.
    void handleLogout(const QTcpSocket* socket, const shared::Packet& packet);
};