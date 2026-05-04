#include "MessageSender.h"

#include "Client.h"
#include "Message.h"
#include "RequestManager.h"

MessageSender::MessageSender(QObject* parent)
    : QObject(parent)
{}

void MessageSender::processMessage(const ChatMessage* message) const
{
    qDebug() << "Processing message:" << message->content();

    while (!Client::instance().connected()) {}

    RequestManager::instance().sendChatMessage(
        shared::Message(shared::MessageType::TEXT, message->content())
    );

    emit messageSent(message->id());
}
