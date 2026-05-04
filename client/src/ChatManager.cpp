#include "ChatManager.h"

#include <QSet>

#include <utility>

#include "AccountManager.h"
#include "Chat.h"
#include "RequestManager.h"

ChatManager::ChatManager(QObject* parent)
    : QObject(parent),
      m_selectedChat(nullptr)
{
    connect(&AccountManager::instance(), &AccountManager::loggedInChanged, this, &ChatManager::onLoggedInChanged);
    connect(&RequestManager::instance(), &RequestManager::chatListReceived, this, &ChatManager::onChatListReceived);
}

ChatManager& ChatManager::instance()
{
    static ChatManager instance;
    return instance;
}

void ChatManager::selectChat(const QUuid& chatId)
{
    if (!m_chatStorage.contains(chatId)) return;

    Chat* chat = m_chatStorage.value(chatId);
    if (!chat) return;
    if (m_selectedChat == chat) return;

    m_selectedChat = chat;
    emit selectedChatChanged();
}

void ChatManager::unselectChat()
{
    if (m_selectedChat == nullptr) return;

    m_selectedChat = nullptr;
    emit selectedChatChanged();
}

void ChatManager::addChat(Chat* chat)
{
    if (chat == nullptr || m_chatStorage.contains(chat->id())) return;

    m_chatStorage.insert(chat->id(), chat);
    m_chatList.append(chat);
    emit chatsChanged();
}

void ChatManager::removeChat(const QUuid& chatId)
{
    Chat* chat = m_chatStorage.take(chatId);
    if (chat == nullptr) return;

    const bool wasSelected = (m_selectedChat == chat);
    m_chatList.removeAll(chat);

    if (wasSelected)
        m_selectedChat = nullptr;

    emit chatsChanged();

    if (wasSelected)
        emit selectedChatChanged();
}

void ChatManager::onLoggedInChanged()
{
    if (AccountManager::instance().loggedIn())
    {
        fetchChatList();
    }
    else
    {
        clearChatList();
    }
}

void ChatManager::onChatListReceived(const shared::ChatsInfo& chats)
{
    clearChatList();

    for (const auto& chat : chats.chats())
    {
        QSet<QUuid> memberIds;
        for (const QUuid& memberId : chat.memberIds())
            memberIds.insert(memberId);

        if (const auto& currentUserId = AccountManager::instance().userId(); currentUserId.has_value())
            memberIds.remove(currentUserId.value());

        addChat(new Chat(chat.id(), std::move(memberIds), this));
    }
}

void ChatManager::fetchChatList()
{
    RequestManager::instance().getCurrentUserChats();
}

void ChatManager::clearChatList()
{
    const QList<Chat*> chats = m_chatList;
    for (Chat* chat : chats)
    {
        removeChat(chat->id());
    }
}
