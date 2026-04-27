#include "MessageSender.h"

#include "Client.h"

MessageSender::MessageSender(QObject* parent)
    : QObject(parent)
{}

void MessageSender::processMessage(const ChatMessage* message) const
{
    qDebug() << "Processing message:" << message->content();

    Client& client = Client::instance();

    while (!client.connected()) {}

    client.sendMessage(message->content());

    emit messageSent(message->id());
}
