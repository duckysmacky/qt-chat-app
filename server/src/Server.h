/**
 * @file Server.h
 * @brief Definition of the Server singleton class for TCP server with multi-client broadcasting.
 */

#pragma once

#include <QByteArray>
#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QHash>
#include <optional>
#include <QSet>
#include <cstdint>

#include "Packet.h"
#include "ClientConnection.h"
#include "dto/ProfileInfo.h"
#include "dto/PublicUserInfo.h"
#include "dto/UserInfoRequest.h"
#include "dto/ChatInfo.h"
#include "dto/ChatsInfo.h"
#include "dto/CreateChatInfo.h"

/**
 * @class Server
 * @brief TCP server with console input and multi-client broadcasting.
 * 
 * The Server class implements a multithreaded TCP server that handles client connections,
 * authenticates users, processes various packet types (chat messages, user management,
 * profile operations, chat management), and manages real-time communication between clients.
 * It follows the singleton pattern to ensure a single server instance throughout the application.
 */
class Server : public QObject
{
    Q_OBJECT

private:
    const QUuid m_uuid;                              ///< Server UUID for unique identification.
    QTcpServer* m_server;                            ///< TCP server instance for accepting incoming connections.
    QHash<QUuid, ClientConnection> m_clients;        ///< Hash map of connected clients keyed by session UUID.
    QHash<QTcpSocket*, QByteArray> m_socketBuffers;  ///< Buffer storage for partial data received from each socket.
    bool m_isRunning;                                ///< Server running state flag (true if running, false otherwise).
    void handleKeyExchange(const QTcpSocket* socket, const shared::Packet& packet);
    void sendPublicKey(const QUuid& receiverSessionId) const;

public:
    /**
     * @brief Returns the singleton Server instance.
     * @return Reference to the unique Server instance.
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
     * @brief Destructor. Stops the server and cleans up resources.
     */
    ~Server() override;

    /**
     * @brief Starts the TCP server on the specified port.
     * @param port Port number to listen on (default 8080)
     * @return true if the server started successfully, false otherwise (e.g., port already in use).
     */
    bool start(uint16_t port = 8080);

    /**
     * @brief Stops the server and disconnects all clients.
     */
    void stop() const;

    /**
     * @brief Sends a packet to a specific client.
     * @param receiver UUID of the target client.
     * @param packet The packet to send.
     */
    void sendPacket(const QUuid& receiver, const shared::Packet& packet) const;
    
    /**
     * @brief Sends an error message to a specific client.
     * @param receiver UUID of the target client.
     * @param message The error message text.
     */
    void sendError(const QUuid& receiver, QString message) const;
    
    /**
     * @brief Sends a success message to a specific client.
     * @param receiver UUID of the target client.
     * @param message The success message text.
     */
    void sendSuccess(const QUuid& receiver, QString message) const;

    /**
     * @brief Checks if the server is currently running.
     * @return true if the server is running, false otherwise.
     */
    bool isRunning() const { return m_isRunning; }

public slots:
    /**
     * @brief Handles new incoming TCP connections.
     * 
     * Called when a new client attempts to connect. Accepts the connection,
     * creates a socket, and sets up signal/slot connections for the new client.
     */
    void onNewConnection();

    /**
     * @brief Handles client disconnections.
     * 
     * Called when a client disconnects. Removes the client from the clients hash
     * and cleans up associated resources.
     */
    void onClientDisconnected();

    /**
     * @brief Reads incoming data from clients.
     * 
     * Called when data is available on a client socket. Reads the data,
     * appends it to the appropriate buffer, and processes complete packets.
     */
    void onServerRead();

private:
    /// @brief Private constructor for singleton pattern.
    /// @param parent Optional QObject parent pointer (default nullptr).
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

    /// @brief Handles a CONNECT_CLIENT packet for initial client handshake.
    /// @param socket The client socket.
    /// @param packet The received packet containing client information.
    void handleConnectClient(QTcpSocket* socket, const shared::Packet& packet);

    /// @brief Handles a chat message packet from an authenticated client.
    /// @param connection The client connection.
    /// @param packet The received packet containing the chat message.
    void handleChatMessage(const ClientConnection& connection, const shared::Packet& packet) const;

    /// @brief Handles a REGISTER_USER packet for new user registration.
    /// @param socket The client socket.
    /// @param packet The received packet containing registration details.
    void handleRegisterUser(const QTcpSocket* socket, const shared::Packet& packet);

    /// @brief Handles a LOGIN_USER packet for user authentication.
    /// @param socket The client socket.
    /// @param packet The received packet containing login credentials.
    void handleLoginUser(const QTcpSocket* socket, const shared::Packet& packet);

    /// @brief Handles an authorized packet (requires valid authentication).
    /// @param packet The received packet to process if properly authorized.
    void handleAuthorizedPacket(const shared::Packet& packet) const;

    /// @brief Handles a LOGOUT_USER packet to log out a user.
    /// @param socket The client socket.
    /// @param packet The received logout request packet.
    void handleLogoutUser(const QTcpSocket* socket, const shared::Packet& packet);

    /// @brief Handles a GET_USER_PROFILE packet to request user profile data.
    /// @param socket The client socket.
    /// @param packet The received packet containing the profile request.
    void handleGetUserProfile(const QTcpSocket* socket, const shared::Packet& packet);

    /// @brief Handles an UPDATE_USER_PROFILE packet to modify user profile.
    /// @param socket The client socket.
    /// @param packet The received packet containing updated profile information.
    void handleUpdateUserProfile(const QTcpSocket* socket, const shared::Packet& packet);

    /// @brief Handles a GET_USER_INFO packet to request user information.
    /// @param socket The client socket.
    /// @param packet The received packet containing the user info request.
    void handleGetUserInfo(const QTcpSocket* socket, const shared::Packet& packet);

    /**
     * @brief Sends user profile data to a specific client.
     * @param receiverSessionId The session UUID of the receiving client.
     * @param info The profile information to send.
     */
    void sendUserProfileData(const QUuid& receiverSessionId, const shared::ProfileInfo& info) const;
    
    /**
     * @brief Sends public user information to a specific client.
     * @param receiverSessionId The session UUID of the receiving client.
     * @param info The public user information to send.
     */
    void sendPublicUserInfoData(const QUuid& receiverSessionId, const shared::PublicUserInfo& info) const;

    /**
     * @brief Sends a packet with encrypted payload to a specific client.
     * @param receiverSessionId The session UUID of the receiving client.
     * @param packet The packet whose payload should be encrypted before sending.
     */
    void sendEncryptedPacket(const QUuid& receiverSessionId, const shared::Packet& packet) const;

    /// @brief Handles a GET_CHATS packet to request the user's chat list.
    /// @param socket The client socket.
    /// @param packet The received packet containing the chat list request.
    void handleGetChats(const QTcpSocket* socket, const shared::Packet& packet);

    /// @brief Handles a SEARCH_CHATS packet to search for chats.
    /// @param socket The client socket.
    /// @param packet The received packet containing search criteria.
    void handleSearchChats(const QTcpSocket* socket, const shared::Packet& packet);

    /// @brief Handles a CREATE_CHAT packet to create a new chat.
    /// @param socket The client socket.
    /// @param packet The received packet containing chat creation details.
    void handleCreateChat(const QTcpSocket* socket, const shared::Packet& packet);

    /**
     * @brief Sends chat list data to a specific client.
     * @param receiverSessionId The session UUID of the receiving client.
     * @param info The chat list information to send.
     */
    void sendChatListData(const QUuid& receiverSessionId, const shared::ChatsInfo& info) const;
    
    /**
     * @brief Sends detailed chat information to a specific client.
     * @param receiverSessionId The session UUID of the receiving client.
     * @param info The detailed chat information to send.
     */
    void sendChatInfoData(const QUuid& receiverSessionId, const shared::ChatInfo& info) const;
    
    /**
     * @brief Sends updated chat lists to all members of affected chats.
     * @param memberUserIds Set of user IDs who should receive updated chat lists.
     * 
     * Used to notify all participants when a chat is created or modified.
     */
    void sendUpdatedChatLists(const QSet<QUuid>& memberUserIds) const;
};
