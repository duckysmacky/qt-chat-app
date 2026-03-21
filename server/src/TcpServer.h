#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QHash>
#include <QUuid>

#include "ConsoleReader.h"

#include <cstdint>

#include "Message.h"

/**
 * @class TcpServer
 * @brief TCP server with console input and multi-client broadcasting.
 */
class TcpServer : public QObject
{
    Q_OBJECT

private:
    const QUuid m_uuid; ///< Server UUID
    QTcpServer* m_server; ///< TCP server
    QHash<QUuid, QTcpSocket*> m_sockets; ///< Connected clients
    ConsoleReader* m_consoleReader; ///< Console input reader
    bool m_isRunning; ///< Server running state (true/false)

public:
     /**
     * @brief Returns the singleton TcpServer instance
     * @return Reference to the server
     */
    static TcpServer& instance();

    /**
     * @brief Constructors for TCP server
     */
    TcpServer(const TcpServer& other) = delete;
    TcpServer& operator =(const TcpServer& other) = delete;
    TcpServer(TcpServer&& other) = delete;
    TcpServer& operator =(TcpServer&& other) = delete;

    /**
     * @brief Destructor. Stops the server and console reader.
     */
    ~TcpServer() override;

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
     * @brief Sends a message to a specific client.
     * @param target Client UUID
     * @param msg Message to send
     */
    void sendMessage(const QUuid& target, const shared::Message& msg) const;
    /**
     * @brief Broadcasts a text message to all connected clients.
     * @param text Message content
     */
    void broadcast(const QString& text) const;

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
    explicit TcpServer(QObject* parent = nullptr);
    void handleMessage(const shared::Message& msg) const;
    void registerClient(QTcpSocket* socket, const shared::Message& msg);
};
