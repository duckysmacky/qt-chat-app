#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QHash>

#include <cstdint>

#include "Packet.h"
#include "ClientConnection.h"
#include "dto/ProfileInfo.h"
#include "dto/PublicUserInfo.h"
#include "dto/ChatInfo.h"
#include "dto/ChatsInfo.h"
#include "dto/CreateChatInfo.h"

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

    /// @brief Handles a CONNECT_CLIENT packet.
    /// @param socket The client socket.
    /// @param packet The received packet.
    void handleConnectClient(QTcpSocket* socket, const shared::Packet& packet);

    /// @brief Handles a chat message packet from an authenticated client.
    /// @param connection The client connection.
    /// @param packet The received packet.
    void handleChatMessage(const ClientConnection& connection, const shared::Packet& packet) const;

    /// @brief Handles a REGISTER_USER packet.
    /// @param socket The client socket.
    /// @param packet The received packet.
    void handleRegisterUser(const QTcpSocket* socket, const shared::Packet& packet);

    /// @brief Handles a LOGIN_USER packet.
    /// @param socket The client socket.
    /// @param packet The received packet.
    void handleLoginUser(const QTcpSocket* socket, const shared::Packet& packet);

    /// @brief Handles an authorized packet (requires authentication).
    /// @param packet The received packet.
    void handleAuthorizedPacket(const shared::Packet& packet) const;

    /// @brief Handles a LOGOUT_USER packet.
    /// @param socket The client socket.
    /// @param packet The received packet.
    void handleLogoutUser(const QTcpSocket* socket, const shared::Packet& packet);

    /// @brief Handles a GET_USER_PROFILE packet.
    void handleGetUserProfile(const QTcpSocket* socket, const shared::Packet& packet);

    /// @brief Handles an UPDATE_USER_PROFILE packet.
    void handleUpdateUserProfile(const QTcpSocket* socket, const shared::Packet& packet);

    /// @brief Handles a GET_PUBLIC_USER_INFO packet.
    void handleGetPublicUserInfo(const QTcpSocket* socket, const shared::Packet& packet);

    void sendUserProfileData(const QUuid& receiverSessionId, const shared::ProfileInfo& info) const;
    void sendPublicUserInfoData(const QUuid& receiverSessionId, const shared::PublicUserInfo& info) const;

    /// @brief Handles a GET_CHATS packet.
    void handleGetChats(const QTcpSocket* socket, const shared::Packet& packet);

    /// @brief Handles a SEARCH_CHATS packet.
    void handleSearchChats(const QTcpSocket* socket, const shared::Packet& packet);

    /// @brief Handles a CREATE_CHAT packet.
    void handleCreateChat(const QTcpSocket* socket, const shared::Packet& packet);

    void sendChatListData(const QUuid& receiverSessionId, const shared::ChatsInfo& info) const;
    void sendChatInfoData(const QUuid& receiverSessionId, const shared::ChatInfo& info) const;


};
