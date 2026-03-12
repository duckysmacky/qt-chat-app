#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QStringList>

class Client : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ connected NOTIFY connectionStatusChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(QStringList messages READ messages NOTIFY messagesChanged)

private:
    QTcpSocket m_socket;
		bool m_connected;
    QString m_statusText;
    QStringList m_messages;

public:
    explicit Client(QObject* parent = nullptr);

    Q_INVOKABLE void connectTo(const QString& host, int port);
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE void sendMessage(const QString& text);

		bool connected() const { return m_connected; }
    const QString& statusText() const { return m_statusText; }
    const QStringList& messages() const { return m_messages; }

signals:
    void connectionStatusChanged();
    void statusTextChanged();
    void messagesChanged();
    void messageReceived(const QString& message);

private slots:
    void onConnected();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError error);
    void onReadyRead();

private:
		void setConnectionStatus(bool connected);
    void setStatusText(const QString& text);
    void appendMessage(const QString& message);
};
