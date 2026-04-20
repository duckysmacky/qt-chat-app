#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QUuid>

#include "Packet.h"
#include "Message.h"

/**
 * @class Client
 * @brief TCP client for connecting to the server and exchanging messages.
 * @details
 * Provides connection management, message sending, and receiving.
 * @note Uses singleton pattern; access via instance().
 */
class Client : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ connected NOTIFY connectionStatusChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)

private:
    const QUuid m_uuid;          ///< Unique identifier for this client instance
    QTcpSocket m_socket;         ///< TCP socket for server communication
    bool m_connected;            ///< Current connection state
    QString m_statusText;        ///< Human-readable status message

private:
    /**
     * @brief Private constructor (singleton pattern).
     * @param parent Parent QObject (default nullptr).
     */
    explicit Client(QObject* parent = nullptr);

public:
    /**
     * @brief Returns the singleton client instance.
     * @return Reference to the Client instance.
     */
    static Client& instance();

    /// @brief Deleted copy constructor (singleton pattern).
    Client(const Client&) = delete;

    /// @brief Deleted copy assignment operator (singleton pattern).
    Client& operator =(const Client&) = delete;

    /// @brief Deleted move constructor (singleton pattern).
    Client(Client&&) = delete;

    /// @brief Deleted move assignment operator (singleton pattern).
    Client& operator =(Client&&) = delete;

    /**
     * @brief Connects the client to a server.
     * @param host Server's hostname or IP address.
     * @param port Server's port number.
     */
    Q_INVOKABLE void connectTo(const QString& host, int port);

    /**
     * @brief Disconnects the client from the server.
     */
    Q_INVOKABLE void disconnect();

    /**
     * @brief Sends a text message to the server.
     * @param content Message content.
     */
    Q_INVOKABLE void sendMessage(QString content);

    /**
     * @brief Authenticates a user with the server.
     * @param login Username or email.
     * @param passwordHash Hashed password.
     */
    void login(QString login, QString passwordHash);

    /**
     * @brief Registers a new user account with the server.
     * @param username Desired username.
     * @param name Display name.
     * @param email Email address.
     * @param passwordHash Hashed password.
     */
    void registerUser(QString username, QString name, QString email, QString passwordHash);

    /// @brief Returns the unique identifier of this client.
    /// @return Constant reference to the client UUID.
    const QUuid& uuid() const { return m_uuid; }

    /// @brief Checks if the client is currently connected to the server.
    /// @return true if connected, false otherwise.
    bool connected() const { return m_connected; }

    /// @brief Returns the current status text.
    /// @return Constant reference to the status string.
    const QString& statusText() const { return m_statusText; }

signals:
    void connectionStatusChanged();                           ///< Emitted when connection state changes.
    void statusTextChanged();                                 ///< Emitted when status text changes.
    void messageReceived(const QString& sender, const shared::Message& messagePacket); ///< Emitted when a new message is received.
    void resultReceived(bool success, const QString& message); ///< Emitted when an operation result is received.

private slots:
    void onConnected();          ///< Handles successful connection to the server.
    void onDisconnected();       ///< Handles disconnection from the server.
    void onErrorOccurred(QAbstractSocket::SocketError error); ///< Handles socket errors.
    void onReadyRead();          ///< Handles incoming data from the server.

private:
    /**
     * @brief Sends a packet with only a type (no data).
     * @param type The packet type to send.
     */
    void sendPacket(shared::PacketType type);

    /**
     * @brief Sends a packet with type and data.
     * @param type The packet type to send.
     * @param data The serialized data to attach.
     */
    void sendPacket(shared::PacketType type, QByteArray data);

    /**
     * @brief Sets the connection state and emits the appropriate signal.
     * @param connected New connection state.
     */
    void setConnectionStatus(bool connected);

    /**
     * @brief Sets the status text and emits the statusTextChanged signal.
     * @param text New status text.
     */
    void setStatusText(const QString& text);
};