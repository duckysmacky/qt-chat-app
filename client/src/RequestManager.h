/**
 * @file RequestManager.h
 * @brief Definition of the RequestManager singleton class for packet routing and building.
 */

#pragma once

#include <QByteArray>
#include <QList>
#include <QObject>
#include <QSet>
#include <QString>
#include <QUuid>

#include <optional>

#include "Message.h"
#include "OperationResult.h"
#include "Packet.h"
#include "dto/ChatInfo.h"
#include "dto/ChatsInfo.h"
#include "dto/CreateChatInfo.h"
#include "dto/ProfileInfo.h"
#include "dto/ProfileUpdateInfo.h"
#include "dto/PublicUserInfo.h"
#include "dto/SessionInfo.h"
#include "dto/UserInfoRequest.h"

/**
 * @class RequestManager
 * @brief Routes incoming packets and builds outgoing packets for the client
 * 
 * The RequestManager serves as the central communication hub for the client application.
 * It processes incoming byte streams into structured packets, routes them to appropriate
 * handlers, and provides methods for constructing and sending various types of server requests.
 * 
 * @note Uses Meyers singleton pattern; access via instance().
 */
class RequestManager : public QObject
{
    Q_OBJECT

private:
    struct QueuedPacket
    {
        shared::Packet packet;
        QByteArray payload;
    };

    QByteArray m_incomingBuffer; ///< Buffer for accumulating incoming byte data before packet parsing.
    mutable QList<QueuedPacket> m_packetQueue;
    mutable QSet<QUuid> m_requestedKeyExchanges;

public:
    /**
     * @brief Returns the singleton instance of the RequestManager.
     * @return Reference to the unique RequestManager instance.
     */
    static RequestManager& instance();

    // Deleted copy/move constructors and assignment operators to enforce singleton pattern.
    RequestManager(const RequestManager&) = delete;
    RequestManager& operator =(const RequestManager&) = delete;
    RequestManager(RequestManager&&) = delete;
    RequestManager& operator =(RequestManager&&) = delete;

    /**
     * @brief Processes raw bytes received from the network connection.
     * @param bytes The incoming byte array to be processed.
     * 
     * Appends the bytes to the internal buffer and attempts to parse complete packets.
     * When a complete packet is identified, it is passed to processPacket() for handling.
     */
    void processBytes(const QByteArray& bytes);

    /**
     * @brief Establishes connection to the server.
     * 
     * Initiates the network connection to the backend server.
     */
    void connectClient() const;
    
    /**
     * @brief Sends a server command without additional data.
     * 
     * Sends an empty or minimal command packet to the server.
     */
    void sendServerCommand() const;
    
    /**
     * @brief Sends a server command with associated data.
     * @param data The payload data to accompany the command.
     */
    void sendServerCommand(QByteArray data) const;
    
    /**
     * @brief Sends a chat message to the server.
     * @param message The complete Message object to be sent.
     */
    void sendChatMessage(shared::Message message) const;
    
    /**
     * @brief Sends a text-only chat message.
     * @param targetChatId The unique identifier of the target chat.
     * @param content The text content of the message.
     */
    void sendTextChatMessage(const QUuid& targetChatId, QString content) const;

    /**
     * @brief Sends a P2P-encrypted text message for a direct chat.
     * @param targetChatId The unique identifier of the target chat.
     * @param content The text content of the message.
     * @param receiverPublicKey RSA public key of the receiver for message content encryption.
     */
    void sendTextChatMessage(const QUuid& targetChatId, QString content, const QByteArray& receiverPublicKey) const;

    /**
     * @brief Sends a media chat message (e.g., image, video, file).
     * @param targetChatId The unique identifier of the target chat.
     * @param content The media content or reference to media.
     */
    void sendMediaChatMessage(const QUuid& targetChatId, QString content) const;

    /**
     * @brief Sends a P2P-encrypted media message for a direct chat.
     */
    void sendMediaChatMessage(const QUuid& targetChatId, QString content, const QByteArray& receiverPublicKey) const;

    /**
     * @brief Proactively initiates RSA public key exchange with a peer.
     * @param peerSessionId Session ID of the peer.
     *
     * Call this when opening a direct chat to ensure the peer's public key
     * is available before the first message is sent.
     */
    void initiatePeerKeyExchange(const QUuid& peerSessionId) const;
    
    /**
     * @brief Authenticates a user with the server.
     * @param login The user's login identifier.
     * @param passwordHash The hashed password for authentication.
     */
    void loginUser(QString login, QString passwordHash) const;
    
    /**
     * @brief Registers a new user account.
     * @param username Desired unique username.
     * @param displayName Display name for the user profile.
     * @param email User's email address.
     * @param passwordHash Hashed password for the new account.
     */
    void registerUser(QString username, QString displayName, QString email, QString passwordHash) const;
    
    /**
     * @brief Logs out the currently authenticated user.
     */
    void logoutCurrentUser() const;
    
    /**
     * @brief Requests the profile information of the currently logged-in user.
     */
    void getCurrentUserProfile() const;
    
    /**
     * @brief Updates the profile of the currently logged-in user.
     * @param info The profile update information containing fields to be updated.
     */
    void updateCurrentUserProfile(shared::ProfileUpdateInfo info) const;
    
    /**
     * @brief Requests user information based on the provided request criteria.
     * @param request The UserInfoRequest object containing search/request parameters.
     */
    void getUserInfo(shared::UserInfoRequest request) const;
    
    /**
     * @brief Requests user information by user ID.
     * @param userId The unique identifier of the requested user.
     */
    void getUserInfo(const QUuid& userId) const;
    
    /**
     * @brief Requests user information by username.
     * @param username The username of the requested user.
     */
    void getUserInfo(QString username) const;
    
    /**
     * @brief Requests public user information by user ID.
     * @param userId The unique identifier of the user.
     * 
     * Unlike getUserInfo(), this method returns only publicly available information.
     */
    void getPublicUserInfo(const QUuid& userId) const;

    void getUserSession(const QUuid& userId) const;
    
    /**
     * @brief Requests the list of chats for the currently logged-in user.
     */
    void getCurrentUserChats() const;
    
    /**
     * @brief Searches for chats matching the provided query string.
     * @param query The search query string.
     */
    void searchChats(QString query) const;
    
    /**
     * @brief Creates a new chat with the specified configuration.
     * @param info The ChatCreateInfo object containing chat creation parameters.
     */
    void createChat(shared::ChatCreateInfo info) const;
    
    /**
     * @brief Creates a new chat with the specified member IDs.
     * @param memberIds List of user IDs to include as members in the chat.
     */
    void createChat(QList<QUuid> memberIds) const;

signals:
    /**
     * @brief Emitted when a chat message is received from the server.
     * @param message The received Message object.
     */
    void chatMessageReceived(const shared::Message& message);
    
    /**
     * @brief Emitted when an operation result is received from the server.
     * @param result The OperationResult containing success/failure information.
     */
    void operationResultReceived(const shared::OperationResult& result);
    
    /**
     * @brief Emitted when the current user's profile is received.
     * @param profile The ProfileInfo containing the user's profile data.
     */
    void currentUserProfileReceived(const shared::ProfileInfo& profile);
    
    /**
     * @brief Emitted when public user information is received.
     * @param userInfo The PublicUserInfo containing publicly available user data.
     */
    void publicUserInfoReceived(const shared::PublicUserInfo& userInfo);
    
    /**
     * @brief Emitted when a list of chats is received from the server.
     * @param chats The ChatsInfo object containing chat list data.
     */
    void chatListReceived(const shared::ChatsInfo& chats);
    
    /**
     * @brief Emitted when detailed chat information is received.
     * @param chat The ChatInfo object containing chat details.
     */
    void chatInfoReceived(const shared::ChatInfo& chat);

    void userSessionReceived(const shared::SessionInfo& sessionInfo);
    
    /**
     * @brief Emitted when a packet is received but its format is invalid.
     * @param packet The invalid packet that was received.
     */
    void invalidPacketReceived(const shared::Packet& packet);
    
    /**
     * @brief Emitted when a packet of an unsupported type is received.
     * @param packet The unsupported packet that was received.
     */
    void unsupportedPacketReceived(const shared::Packet& packet);

private:
    /**
     * @brief Private constructor for singleton pattern.
     * @param parent Optional QObject parent pointer.
     */
    explicit RequestManager(QObject* parent = nullptr);

    /**
     * @brief Processes a complete, parsed packet.
     * @param packet The packet to be processed and routed.
     * 
     * Examines the packet type and emits the appropriate signal
     * for further handling by other components.
     */
    void processPacket(const shared::Packet& packet);
    
    /**
     * @brief Sends a packet to the server.
     * @param packet The packet to be serialized and transmitted.
     * 
     * Converts the packet to a byte array and sends it through the network connection.
     */
    void sendPlainPacket(shared::Packet packet) const;

    void sendEncryptedPacket(shared::Packet packet, QByteArray payload) const;
    void requestKeyExchange(const QUuid& receiverSessionId) const;
    void handleKeyExchange(const shared::Packet& packet);
    void flushQueuedPackets(const QUuid& receiverSessionId) const;
    std::optional<QByteArray> decryptPayload(const shared::Packet& packet) const;
};
