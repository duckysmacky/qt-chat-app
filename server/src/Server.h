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
 * @brief TCP server with multi-client support and console-based control.
 */
class Server : public QObject
{
    Q_OBJECT

private:
    const QUuid m_uuid; ///< Server unique identifier
    QTcpServer* m_server; ///< Underlying Qt TCP server
    QHash<QUuid, ClientConnection> m_clients; ///< Active client connections
    bool m_isRunning; ///< Indicates whether the server is running

public:
    /**
     * @brief Returns the singleton instance of the Server.
     * @return Reference to the Server instance.
     */
    static Server& instance();

    Server(const Server& other) = delete;
    Server& operator=(const Server& other) = delete;
    Server(Server&& other) = delete;
    Server& operator=(Server&& other) = delete;

    /**
     * @brief Destructor. Stops the server and releases resources.
     */
    ~Server() override;

    /**
     * @brief Starts the TCP server.
     * @param port Port number (default is 8080).
     * @return true if the server started successfully.
     */
    bool start(uint16_t port = 8080);

    /**
     * @brief Stops the server and disconnects all clients.
     */
    void stop() const;

    /**
     * @brief Sends a packet to a specific client.
     * @param target UUID of the target client.
     * @param packet Packet to send.
     */
    void sendPacket(const QUuid& target, const shared::Packet& packet) const;

    /**
     * @brief Sends an error message to a specific client.
     * @param target UUID of the target client.
     * @param message Error message.
     */
    void sendError(const QUuid& target, QString message) const;

    /**
     * @brief Sends a success message to a specific client.
     * @param target UUID of the target client.
     * @param message Success message.
     */
    void sendSuccess(const QUuid& target, QString message) const;

    /**
     * @brief Checks whether the server is running.
     * @return true if the server is active.
     */
    bool isRunning() const { return m_isRunning; }

public slots:
    /**
     * @brief Handles new incoming TCP connections.
     */
    void onNewConnection();

    /**
     * @brief Handles client disconnection events.
     */
    void onClientDisconnected();

    /**
     * @brief Reads incoming data from connected clients.
     */
    void onServerRead();

private:
    explicit Server(QObject* parent = nullptr);

    std::optional<std::reference_wrapper<ClientConnection>> findConnection(const QUuid& sessionId);
    std::optional<std::reference_wrapper<const ClientConnection>> findConnection(const QUuid& sessionId) const;
    std::optional<std::reference_wrapper<ClientConnection>> findConnection(const QTcpSocket* clientSocket);
    std::optional<std::reference_wrapper<const ClientConnection>> findConnection(const QTcpSocket* clientSocket) const;

    void handleConnect(QTcpSocket* socket, const shared::Packet& packet);
    void handleChatMessage(const ClientConnection& connection, const shared::Packet& packet) const;
    void handleRegister(const QTcpSocket* socket, const shared::Packet& packet);
    void handleLogin(const QTcpSocket* socket, const shared::Packet& packet);
    void handleAuthorizedPacket(const shared::Packet& packet) const;
    void handleLogout(const QTcpSocket* socket, const shared::Packet& packet);
};