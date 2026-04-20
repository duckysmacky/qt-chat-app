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
    const QUuid m_uuid;
    QTcpSocket m_socket;
    bool m_connected;
    QString m_statusText;

private:
    explicit Client(QObject* parent = nullptr);

public:
    /**
     * @brief Returns the singleton client instance
     */
    static Client& instance();

    /// Copy and move are disabled (singleton)
    Client(const Client&) = delete;
    Client& operator =(const Client&) = delete;
    Client(Client&&) = delete;
    Client& operator =(Client&&) = delete;

    /**
     * @brief Connect client to a server
     * @param host Server's host
     * @param port Server's port
     */
    Q_INVOKABLE void connectTo(const QString& host, int port);

    /**
     * @brief Disconnects client from a server
     */
    Q_INVOKABLE void disconnect();

    /**
     * @brief Sends a message to the server.
     * @param content Message content
     */
    Q_INVOKABLE void sendMessage(QString content);

    void login(QString login, QString passwordHash);
    void registerUser(QString username, QString name, QString email, QString passwordHash);

    const QUuid& uuid() const { return m_uuid; }
    bool connected() const { return m_connected; }
    const QString& statusText() const { return m_statusText; }

signals:
    void connectionStatusChanged();
    void statusTextChanged();
    void messageReceived(const QString& sender, const shared::Message& messagePacket);
    void resultReceived(bool success, const QString& message);

private slots:
    void onConnected();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError error);
    void onReadyRead();

private:
    void sendPacket(shared::PacketType type);
    void sendPacket(shared::PacketType type, QByteArray data);
    void setConnectionStatus(bool connected);
    void setStatusText(const QString& text);
};
