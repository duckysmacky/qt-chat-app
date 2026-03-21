#pragma once

#include <QObject>
#include <QStringList>

#include "Message.h"


/**
 * @class ChatWindow
 * @brief UI-facing class for managing and displaying chat messages.
 * Stores received and sent messages and exposes them
 */
class ChatWindow : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList messages READ messages NOTIFY messagesChanged)

private:
    QStringList m_messages; ///<List of chat messages

public:
    /**
     * @brief Constructs the ChatWindow.
     */
    explicit ChatWindow(QObject* parent = nullptr);

    Q_INVOKABLE void sendMessage(const QString& text);
    /// @brief Returns the list of messages
    const QStringList& messages() const { return m_messages; }

signals:
    /// @brief Signal is emitted when the message list changes
    void messagesChanged();

private slots:
    void onMessageReceived(const shared::Message& msg);

private:
    void appendMessage(const QString& sender, const QString& text);
};
