#pragma once

#include <QObject>
#include <QByteArray>
#include <QTcpSocket>
#include <QUuid>

/**
 * @class Client
 * @brief TCP client for connecting to the server and exchanging bytes.
 * @details
 * Provides connection management and raw byte transport.
 * @note Uses singleton pattern; access via instance().
 */
class Client : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ connected NOTIFY connectionStatusChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)

private:
    const QUuid m_sessionId;     ///< Unique identifier for this client instance
    QUuid m_serverId;            ///< Server session ID
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

    QString resolveUserData(const QUuid& userId) const;

    void updateServerId(const QUuid& serverId);

    /// @brief Returns the unique identifier of this client.
    /// @return Constant reference to the client UUID.
    const QUuid& sessionId() const { return m_sessionId; }

    const QUuid& serverId() const { return m_serverId; }

    /// @brief Checks if the client is currently connected to the server.
    /// @return true if connected, false otherwise.
    bool connected() const { return m_connected; }

    /// @brief Returns the current status text.
    /// @return Constant reference to the status string.
    const QString& statusText() const { return m_statusText; }

    void sendBytes(QByteArray bytes);

signals:
    void connectionStatusChanged();                           ///< Emitted when connection state changes.
    void statusTextChanged();                                 ///< Emitted when status text changes.

private slots:
    void onConnected();          ///< Handles successful connection to the server.
    void onDisconnected();       ///< Handles disconnection from the server.
    void onErrorOccurred(QAbstractSocket::SocketError error); ///< Handles socket errors.
    void onReadyRead();          ///< Handles incoming data from the server.

private:
    void writeBytes(const QByteArray& bytes);

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