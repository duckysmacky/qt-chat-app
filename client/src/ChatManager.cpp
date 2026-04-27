#include "ChatManager.h"

#include "AccountManager.h"
#include "Chat.h"

ChatManager::ChatManager(QObject* parent)
    : QObject(parent),
      m_selectedChat(nullptr)
{
    connect(&AccountManager::instance(), &AccountManager::loggedInChanged, this, &ChatManager::onLoggedInChanged);
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

void ChatManager::fetchChatList()
{
    // TODO
    for (int i = 0; i < 5; i++)
    {
        addChat(new Chat(QUuid::createUuid(), QSet({QUuid::createUuid()}), this));
    }
}

void ChatManager::clearChatList()
{
    const QList<Chat*> chats = m_chatList;
    for (Chat* chat : chats)
    {
        removeChat(chat->id());
    }
}
