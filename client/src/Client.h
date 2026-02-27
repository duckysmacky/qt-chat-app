#pragma once

#include <QObject>
#include <QTcpSocket>

class Client : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(QString lastMessage READ lastMessage NOTIFY lastMessageChanged)

private:
    QTcpSocket m_socket;
    QString m_statusText;
    QString m_lastMessage;

public:
    explicit Client(QObject* parent = nullptr);

    Q_INVOKABLE void connectTo(const QString& host, int port);

    const QString& statusText() const { return m_statusText; }
    const QString& lastMessage() const { return m_lastMessage; }

signals:
    void statusTextChanged();
    void lastMessageChanged();
    void messageReceived(const QString& message);

private slots:
    void onConnected();
    void onErrorOccurred(QAbstractSocket::SocketError error);
    void onReadyRead();

private:
    void setStatusText(const QString& text);
    void setLastMessage(const QString& message);
};
