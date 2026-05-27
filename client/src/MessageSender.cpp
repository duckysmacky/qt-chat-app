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
{}

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
            qWarning() << "Cannot send encrypted message: receiver public key not yet available";
            return;
        }

        RequestManager::instance().sendTextChatMessage(m_chatId, message->content(), receiverPublicKey.value());
    }

    emit messageSent(message->id());
}
