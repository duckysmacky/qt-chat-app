#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QUuid>

#include "Message.h"

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
    static Client& instance();

    Client(const Client&) = delete;
    Client& operator =(const Client&) = delete;
    Client(Client&&) = delete;
    Client& operator =(Client&&) = delete;

    Q_INVOKABLE void connectTo(const QString& host, int port);
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE void sendMessage(shared::MessageType type, const QString& content = "");

    const QUuid& uuid() const { return m_uuid; }
    bool connected() const { return m_connected; }
    const QString& statusText() const { return m_statusText; }

signals:
    void connectionStatusChanged();
    void statusTextChanged();
    void messageReceived(const shared::Message& msg);

private slots:
    void onConnected();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError error);
    void onReadyRead();

private:
    void setConnectionStatus(bool connected);
    void setStatusText(const QString& text);
};
