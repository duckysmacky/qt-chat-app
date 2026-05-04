#pragma once

#include <QObject>
#include <QUuid>

#include "ChatMessage.h"

class MessageSender : public QObject
{
    Q_OBJECT

private:
    QUuid m_chatId;

public:
    explicit MessageSender(QUuid chatId, QObject* parent = nullptr);

public slots:
    void processMessage(const ChatMessage* message) const;

signals:
    void messageSent(const QUuid& messageId) const;
};
