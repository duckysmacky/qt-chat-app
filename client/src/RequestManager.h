#pragma once

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QUuid>

#include "Message.h"
#include "OperationResult.h"
#include "Packet.h"
#include "dto/ChatInfo.h"
#include "dto/ChatsInfo.h"
#include "dto/CreateChatInfo.h"
#include "dto/ProfileInfo.h"
#include "dto/ProfileUpdateInfo.h"
#include "dto/PublicUserInfo.h"

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
    void sendServerCommand() const;
    void sendServerCommand(QByteArray data) const;
    void sendChatMessage(shared::Message message) const;
    void sendTextChatMessage(const QUuid& targetChatId, QString content) const;
    void sendMediaChatMessage(const QUuid& targetChatId, QString content) const;
    void loginUser(QString login, QString passwordHash) const;
    void registerUser(QString username, QString name, QString email, QString passwordHash) const;
    void logoutCurrentUser() const;
    void getCurrentUserProfile() const;
    void updateCurrentUserProfile(shared::ProfileUpdateInfo info) const;
    void getPublicUserInfo(const QUuid& userId) const;
    void getCurrentUserChats() const;
    void searchChats(QString query) const;
    void createChat(shared::ChatCreateInfo info) const;
    void createChat(QString type, QString title) const;

signals:
    void chatMessageReceived(const shared::Message& message);
    void operationResultReceived(const shared::OperationResult& result);
    void currentUserProfileReceived(const shared::ProfileInfo& profile);
    void publicUserInfoReceived(const shared::PublicUserInfo& userInfo);
    void chatListReceived(const shared::ChatsInfo& chats);
    void chatInfoReceived(const shared::ChatInfo& chat);
    void invalidPacketReceived(const shared::Packet& packet);
    void unsupportedPacketReceived(const shared::Packet& packet);

private:
    explicit RequestManager(QObject* parent = nullptr);

    void processPacket(const shared::Packet& packet);
    void sendPacket(shared::Packet packet) const;
};
