#pragma once

#include <QObject>

#include "ChatMessage.h"

class MessageSender : public QObject
{
    Q_OBJECT

public:
    explicit MessageSender(QObject* parent = nullptr);

public slots:
    void processMessage(const ChatMessage* message) const;

signals:
    void messageSent(const QUuid& messageId) const;
};