#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QHash>
#include "ConsoleReader.h"

#include <cstdint>

class TcpServer : public QObject
{
    Q_OBJECT

private:
    QTcpServer* m_server;
    QHash<qintptr, QTcpSocket*> m_sockets;
    ConsoleReader* m_consoleReader;
    bool m_isRunning;

public:
    explicit TcpServer(QObject* parent = nullptr);
    ~TcpServer() override;

	bool start(uint16_t port = 8080);
    void stop() const;
    void broadcast(const QString& message) const;

	bool isRunning() const { return m_isRunning; }

public slots:
    void onNewConnection();
    void onClientDisconnected();
    void onServerRead() const;
};
