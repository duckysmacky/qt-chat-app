#pragma once

#include <QTcpSocket>
#include <QUuid>

#include <optional>

#include "Packet.h"

class ClientConnection
{
private:
    QUuid m_sessionId;
    QTcpSocket* m_socket;
    std::optional<QUuid> m_userId;
    bool m_isAuthorized;

public:
    ClientConnection(const QUuid& sessionId, QTcpSocket* socket);

    void authorize(const QUuid& userId);
    void logout();

    bool matchesSocket(const QTcpSocket* socket) const;
    bool sendPacket(const shared::Packet& packet) const;

    const QUuid& sessionId() const { return m_sessionId; }
    const std::optional<QUuid>& userId() const { return m_userId; }
    bool isAuthorized() const { return m_isAuthorized; }
};
