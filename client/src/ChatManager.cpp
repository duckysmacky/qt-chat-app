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
    connect(&RequestManager::instance(), &RequestManager::chatKeyReceived, this, &ChatManager::onChatKeyReceived);
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

    if (const auto key = m_pendingChatMasterKeys.take(chat->id()); !key.isEmpty())
        chat->setMasterKey(key);

    m_chatStorage.insert(chat->id(), chat);
    m_chatList.append(chat);
    emit chatsChanged();
}

void ChatManager::setChatMasterKey(const QUuid& chatId, QByteArray masterKey)
{
    if (chatId.isNull() || masterKey.isEmpty())
        return;

    if (Chat* chat = m_chatStorage.value(chatId, nullptr))
    {
        chat->setMasterKey(std::move(masterKey));
        return;
    }

    m_pendingChatMasterKeys.insert(chatId, std::move(masterKey));
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

    chat->deleteLater();
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
    const QUuid selectedChatId = m_selectedChat ? m_selectedChat->id() : QUuid();
    QSet<QUuid> receivedChatIds;

    for (const auto& chatInfo : chats.chats())
    {
        receivedChatIds.insert(chatInfo.id());

        QSet<QUuid> memberIds;
        for (const QUuid& memberId : chatInfo.memberIds())
            memberIds.insert(memberId);

        if (const auto& currentUserId = AccountManager::instance().userId(); currentUserId.has_value())
            memberIds.remove(currentUserId.value());

        if (Chat* existingChat = m_chatStorage.value(chatInfo.id(), nullptr))
        {
            existingChat->setOtherMembers(std::move(memberIds));
            continue;
        }

        addChat(new Chat(chatInfo.id(), std::move(memberIds), this));
    }

    const QList<Chat*> currentChats = m_chatList;
    for (Chat* chat : currentChats)
    {
        if (chat != nullptr && !receivedChatIds.contains(chat->id()))
            removeChat(chat->id());
    }

    if (!selectedChatId.isNull() && m_chatStorage.contains(selectedChatId))
        selectChat(selectedChatId);
}

void ChatManager::onChatKeyReceived(const shared::ChatKeyInfo& chatKeyInfo)
{
    setChatMasterKey(chatKeyInfo.chatId(), chatKeyInfo.masterKey());
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
