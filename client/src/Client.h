#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QStringList>

class Client : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(QStringList messages READ messages NOTIFY messagesChanged)

private:
    QTcpSocket m_socket;
    QString m_statusText;
    QStringList m_messages;

public:
    explicit Client(QObject* parent = nullptr);

    Q_INVOKABLE void connectTo(const QString& host, int port);
    Q_INVOKABLE void sendMessage(const QString& message);

    const QString& statusText() const { return m_statusText; }
    const QStringList& messages() const { return m_messages; }

signals:
    void statusTextChanged();
    void messagesChanged();
    void messageReceived(const QString& message);

private slots:
    void onConnected();
    void onErrorOccurred(QAbstractSocket::SocketError error);
    void onReadyRead();

private:
    void setStatusText(const QString& text);
    void appendMessage(const QString& message);
};
