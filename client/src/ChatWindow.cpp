#include "ChatWindow.h"

#include <QRegularExpression>

#include "Client.h"

ChatWindow::ChatWindow(QObject* parent)
    : QObject(parent)
{
    connect(&Client::instance(), &Client::messageReceived, this, &ChatWindow::onMessageReceived);
}

void ChatWindow::sendMessage(const QString& text)
{
    if (text.trimmed().isEmpty()) return;

    qInfo() << "Message sent:" << text;

		appendMessage(text);
    const shared::Message msg(shared::MessageType::Text, text);
    Client::instance().sendMessage(msg);
}

void ChatWindow::onMessageReceived(const shared::Message& msg)
{
		qDebug() << "ON MESSAGE";

    if (msg.type() == shared::MessageType::Text)
    {
				qInfo() << "Incoming text message";

        const QStringList parts = msg.content().split(QRegularExpression("[\\r\\n]+"), Qt::SkipEmptyParts);

        if (parts.isEmpty()) {
            appendMessage(msg.content());
        } else {
            for (const QString& part : parts)
								appendMessage(part);
        }
    }
}

void ChatWindow::appendMessage(const QString& text)
{
    if (text.isEmpty()) return;

    qInfo() << "Displaying message:" << text;

    m_messages.append(text);
    emit messagesChanged();
}
