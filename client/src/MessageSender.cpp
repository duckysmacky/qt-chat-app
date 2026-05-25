#include "MessageSender.h"

#include <utility>

#include "AccountManager.h"
#include "Client.h"
#include "Message.h"
#include "RequestManager.h"

MessageSender::MessageSender(QUuid chatId, QObject* parent)
    : QObject(parent),
      m_chatId(chatId)
{}

void MessageSender::setChatMasterKey(QByteArray chatMasterKey)
{
    m_chatMasterKey = std::move(chatMasterKey);
}

void MessageSender::processMessage(const ChatMessage* message) const
{
    qDebug() << "Processing message:" << message->content();

    if (m_chatMasterKey.isEmpty())
        return;

    const Client& client = Client::instance();
    while (!client.connected()) {}

    RequestManager::instance().sendTextChatMessage(m_chatId, message->content(), m_chatMasterKey);

    emit messageSent(message->id());
}
