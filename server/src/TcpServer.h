#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QHash>
#include <QUuid>

#include "ConsoleReader.h"

#include <cstdint>

#include "Message.h"

class TcpServer : public QObject
{
    Q_OBJECT

private:
    const QUuid m_uuid;
    QTcpServer* m_server;
    QHash<QUuid, QTcpSocket*> m_sockets;
    ConsoleReader* m_consoleReader;
    bool m_isRunning;

public:
    static TcpServer& instance();

    TcpServer(const TcpServer& other) = delete;
    TcpServer& operator =(const TcpServer& other) = delete;
    TcpServer(TcpServer&& other) = delete;
    TcpServer& operator =(TcpServer&& other) = delete;

    ~TcpServer() override;

    bool start(uint16_t port = 8080);
    void stop() const;
    void sendMessage(const QUuid& target, const shared::Message& msg) const;
    void broadcast(const QString& text) const;

    bool isRunning() const { return m_isRunning; }

public slots:
    void onNewConnection();
    void onClientDisconnected();
    void onServerRead();

private:
    explicit TcpServer(QObject* parent = nullptr);
    void handleMessage(const shared::Message& msg) const;
    void registerClient(QTcpSocket* socket, const shared::Message& msg);
};
