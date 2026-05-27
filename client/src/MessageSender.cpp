#include "MessageSender.h"

#include <utility>

#include "Client.h"
#include "KeyStore.h"
#include "RequestManager.h"
#include "SessionResolver.h"

MessageSender::MessageSender(QUuid chatId, QUuid receiverUserId, QObject* parent)
    : QObject(parent),
      m_chatId(std::move(chatId)),
      m_receiverUserId(std::move(receiverUserId))
{
    if (!m_receiverUserId.isNull())
        connect(&RequestManager::instance(), &RequestManager::peerKeyReceived, this, &MessageSender::onPeerKeyReceived);
}

void MessageSender::processMessage(const ChatMessage* message) const
{
    qDebug() << "Processing message:" << message->content();

    while (!Client::instance().connected()) {}

    if (m_receiverUserId.isNull()) // group chat
    {
        RequestManager::instance().sendTextChatMessage(m_chatId, message->content());
    }
    else
    {
        const QUuid receiverSessionId = SessionResolver::instance().userSessionId(m_receiverUserId);
        if (receiverSessionId.isNull())
        {
            qWarning() << "Cannot send encrypted message: receiver session ID not yet known";
            return;
        }

        const auto receiverPublicKey = shared::KeyStore::instance().peerPublicKey(receiverSessionId);
        if (!receiverPublicKey.has_value())
        {
            qInfo() << "Queuing message: receiver public key not yet available";
            m_pendingMessages.enqueue(message);
            return;
        }

        RequestManager::instance().sendTextChatMessage(m_chatId, message->content(), receiverPublicKey.value());
    }

    emit messageSent(message->id());
}

void MessageSender::onPeerKeyReceived(const QUuid& peerSessionId)
{
    if (m_pendingMessages.isEmpty())
        return;

    const QUuid receiverSessionId = SessionResolver::instance().userSessionId(m_receiverUserId);
    if (receiverSessionId != peerSessionId)
        return;

    const auto publicKey = shared::KeyStore::instance().peerPublicKey(peerSessionId);
    if (!publicKey.has_value())
        return;

    while (!m_pendingMessages.isEmpty())
    {
        const ChatMessage* message = m_pendingMessages.dequeue();
        RequestManager::instance().sendTextChatMessage(m_chatId, message->content(), publicKey.value());
        emit messageSent(message->id());
    }
}
