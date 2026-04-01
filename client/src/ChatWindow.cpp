#include "ChatWindow.h"

#include <QRegularExpression>
#include <QTime>

#include "Client.h"

/**
 * @brief Constructs the ChatWindow.
 * @param parent The parent QObject.
 */
ChatWindow::ChatWindow(QObject* parent)
    : QObject(parent)
{
    connect(&Client::instance(), &Client::messageReceived, this, &ChatWindow::onMessageReceived);
}

/**
 * @brief Sends a message to the server.
 * @param text The message text to send.
 */
void ChatWindow::sendMessage(const QString& text)
{
    if (text.trimmed().isEmpty()) return;

    qInfo() << "Sending message:" << text;

    Client& client = Client::instance();
    const QString sender = client.uuid().toString();

    appendMessage(sender, text);
    client.sendMessage(text);
}

/**
 * @brief Handles incoming messages from the server.
 * @param sender The sender of the message
 * @param message The received message.
 */
void ChatWindow::onMessageReceived(const QUuid& sender, const shared::Message& message)
{
    if (message.type() == shared::MessageType::TEXT)
    {
        qInfo() << "Incoming text message from" << sender;

        const QString senderName = sender.toString();
        const QStringList parts = message.content().split(QRegularExpression("[\\r\\n]+"), Qt::SkipEmptyParts);

        if (parts.isEmpty()) {
            appendMessage(senderName, message.content());
        } else {
            for (const QString& part : parts)
                appendMessage(senderName, part);
        }
    }
}

/**
 * @brief Appends a message to the message list.
 * @param sender The sender's identifier.
 * @param text The message text.
 */
void ChatWindow::appendMessage(const QString& sender, const QString& text)
{
    if (text.isEmpty()) return;

    const QString currentTime = QTime::currentTime().toString("hh:mm:ss");
    const QString message = QString("[%1] <%2> %3").arg(currentTime, sender, text);

    qInfo() << "Displaying message:" << message;

    m_messages.append(message);
    emit messagesChanged();
}
