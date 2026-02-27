#pragma once

#include <QTcpServer>
#include <QTcpSocket>

#include <cstdint>

class TcpServer : public QObject
{
    Q_OBJECT

private:
    QTcpServer* m_server;
    QTcpSocket* m_socket;
    bool m_isRunning;

public:
    explicit TcpServer(QObject* parent = nullptr);

	bool start(uint16_t port = 8080);
    void stop() const;

	bool isRunning() const { return m_isRunning; }

public slots:
    void onNewConnection();
    void onClientDisconnected();
    void onServerRead() const;
};
