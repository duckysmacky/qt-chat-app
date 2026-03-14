#pragma once

#include <QObject>
#include <QStringList>

#include "Message.h"

class ChatWindow : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList messages READ messages NOTIFY messagesChanged)

private:
    QStringList m_messages;

public:
    explicit ChatWindow(QObject* parent = nullptr);

    Q_INVOKABLE void sendMessage(const QString& text);

    const QStringList& messages() const { return m_messages; }

signals:
    void messagesChanged();

private slots:
    void onMessageReceived(const shared::Message& msg);

private:
    void appendMessage(const QString& text);
};
