#pragma once

#include <QObject>
#include <QHash>
#include <QUuid>

#include "Chat.h"
#include "dto/ChatsInfo.h"

class ChatManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<Chat*> chats READ chats NOTIFY chatsChanged)
    Q_PROPERTY(Chat* selectedChat READ selectedChat NOTIFY selectedChatChanged)

private:
    QHash<QUuid, Chat*> m_chatStorage;
    QList<Chat*> m_chatList;
    Chat* m_selectedChat;

public:
    static ChatManager& instance();

    ChatManager(const ChatManager&) = delete;
    ChatManager& operator=(const ChatManager&) = delete;
    ChatManager(ChatManager&&) = delete;
    ChatManager& operator=(ChatManager&&) = delete;

    Q_INVOKABLE void selectChat(const QUuid& chatId);
    Q_INVOKABLE void unselectChat();

    void addChat(Chat* chat);
    void removeChat(const QUuid& chatId);

    const QList<Chat*>& chats() const { return m_chatList; }
    Chat* selectedChat() const { return m_selectedChat; }

signals:
    void chatsChanged();
    void selectedChatChanged();

private slots:
    void onLoggedInChanged();
    void onChatListReceived(const shared::ChatsInfo& chats);

private:
    explicit ChatManager(QObject* parent = nullptr);

    void fetchChatList();
    void clearChatList();
};
