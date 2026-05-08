#include "MessageSender.h"

#include <utility>

#include "AccountManager.h"
#include "Client.h"
#include "Message.h"
#include "RequestManager.h"

MessageSender::MessageSender(QUuid chatId, QObject* parent)
    : QObject(parent),
      m_chatId(std::move(chatId))
{}

void MessageSender::processMessage(const ChatMessage* message) const
{
    qDebug() << "Processing message:" << message->content();

    while (!Client::instance().connected()) {}

    QUuid senderUserId;
    if (const auto& userId = AccountManager::instance().userId(); userId.has_value())
        senderUserId = userId.value();

    RequestManager::instance().sendChatMessage(
        shared::Message(senderUserId, m_chatId, shared::MessageType::TEXT, message->content())
    );

    emit messageSent(message->id());
}
