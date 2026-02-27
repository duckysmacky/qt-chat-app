#pragma once

#include <QObject>
#include <QTcpSocket>

class Client : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)

private:
    QTcpSocket m_socket;
    QString m_statusText;

public:
    explicit Client(QObject* parent = nullptr);

    Q_INVOKABLE void connectTo(const QString& host, int port);

    const QString& statusText() const { return m_statusText; }

signals:
    void statusTextChanged();

private slots:
    void onConnected();
    void onErrorOccurred(QAbstractSocket::SocketError error);

private:
    void setStatusText(const QString& text);
};
