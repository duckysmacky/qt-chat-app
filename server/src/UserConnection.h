#pragma once

#include <QTcpSocket>
#include <QUuid>

#include <optional>

class UserConnection
{
public:
    UserConnection();
    UserConnection(const QUuid& sessionId, QTcpSocket* socket);

    const QUuid& sessionId() const;
    QTcpSocket* socket() const;
    const std::optional<QUuid>& userId() const;

    bool isAuthorized() const;
    void authorize(const QUuid& userId);
    void logout();

private:
    QUuid m_sessionId;
    QTcpSocket* m_socket = nullptr;
    std::optional<QUuid> m_userId;
};
