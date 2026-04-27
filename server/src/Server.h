#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QHash>

#include <cstdint>

#include "Packet.h"
#include "ClientConnection.h"
#include "dto\ProfileInfo.h"
#include "dto\PublicUserInfo.h"
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
     * @brief Returns the singleton Server instance
     * @return Reference to the server
     */
    static Server& instance();

    Server(const Server& other) = delete;
    Server& operator =(const Server& other) = delete;
    Server(Server&& other) = delete;
    Server& operator =(Server&& other) = delete;

    /**
     * @brief Destructor. Stops the server and console reader.
     */
    ~Server() override;

    /**
     * @brief Starts the TCP server on the specified port.
     * @param port Port number (default 8080)
     */
	bool start(uint16_t port = 8080);

    /**
     * @brief Stops the server and the console reader.
     */
    void stop() const;

    /**
     * @brief Sends a packet to a specific client.
     * @param receiver Client UUID
     * @param packet Message to send
     */
    void sendPacket(const QUuid& receiver, const shared::Packet& packet) const;
    
	void sendError(const QUuid& receiver, QString message) const;
	void sendSuccess(const QUuid& receiver, QString message) const;

    /**
     * @brief Broadcasts a text message to all connected clients.
     * @param text Message content
     */


    /**
     * @brief Checks if the server is currently running.
     * @return true if server is running
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
    void handleProfileRequest(const QTcpSocket* socket, const shared::Packet& packet);
    void handleProfileUpdate(const QTcpSocket* socket, const shared::Packet& packet);
    void handleUserInfoRequest(const QTcpSocket* socket, const shared::Packet& packet);
    void sendProfileData(const QUuid& receiverSessionId, const shared::ProfileInfo& info) const;
    void sendUserInfoData(const QUuid& receiverSessionId, const shared::PublicUserInfo& info) const;

};
