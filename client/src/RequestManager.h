#pragma once

#include <QByteArray>
#include <QObject>
#include <QString>

#include "Message.h"
#include "Packet.h"
#include "OperationResult.h"

/**
 * @class RequestManager
 * @brief Routes incoming packets and builds outgoing packets for the client
 * @note Uses Meyers singleton pattern; access via instance().
 */
class RequestManager : public QObject
{
    Q_OBJECT

public:
    static RequestManager& instance();

    RequestManager(const RequestManager&) = delete;
    RequestManager& operator =(const RequestManager&) = delete;
    RequestManager(RequestManager&&) = delete;
    RequestManager& operator =(RequestManager&&) = delete;

    void processBytes(const QByteArray& bytes);

    void connectClient() const;
    void sendChatMessage(shared::Message message) const;
    void sendTextChatMessage(QString content) const;
    void loginUser(QString login, QString passwordHash) const;
    void registerUser(QString username, QString name, QString email, QString passwordHash) const;
    void logoutUser() const;

signals:
    void messageReceived(const shared::Message& message);
    void resultReceived(const shared::OperationResult& result);

private:
    explicit RequestManager(QObject* parent = nullptr);

    void processPacket(const shared::Packet& packet);
    void sendPacket(shared::Packet packet) const;
};
