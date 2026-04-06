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

    const QUuid& sessionId() const { return m_sessionId; }
    const std::optional<QUuid>& userId() const { return m_userId; }

    bool isAuthorized() const { return m_userId.has_value(); }
    void authorize(const QUuid& userId);
    void logout();

    bool matchesSocket(const QTcpSocket* socket) const;
    bool writePacket(const shared::Packet& packet) const;

private:
    QUuid m_sessionId;
    QTcpSocket* m_socket = nullptr;
    std::optional<QUuid> m_userId;
};
