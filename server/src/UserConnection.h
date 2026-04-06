#pragma once

#include <QTcpSocket>
#include <QUuid>

#include <optional>

#include "Packet.h"

class UserConnection
{
public:
    UserConnection();
    UserConnection(const QUuid& sessionId, QTcpSocket* socket);

    const QUuid& sessionId() const;
    const std::optional<QUuid>& userId() const;

    bool isAuthorized() const;
    void authorize(const QUuid& userId);
    void logout();

    bool matchesSocket(const QTcpSocket* socket) const;
    bool writePacket(const shared::Packet& packet) const;

private:
    QUuid m_sessionId;
    QTcpSocket* m_socket = nullptr;
    std::optional<QUuid> m_userId;
};
