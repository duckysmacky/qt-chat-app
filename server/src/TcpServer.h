#pragma once

#include <QObject>
#include <QTcpServer>
#include <cstdint>

class TcpServer : public QObject
{
    Q_OBJECT

private:
    QTcpServer* m_server;
    QTcpSocket* m_socket;
    bool m_running;

public:
    explicit TcpServer(QObject* parent = nullptr);
    ~TcpServer();

	void start(uint16_t port = 8080);
	
	bool is_running() const { return m_running; }

public slots:
    void on_new_connection();
    void on_client_disconnected();
    void on_server_read();
};
