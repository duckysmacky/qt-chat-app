#include "ChatCreator.h"

#include <QVariantMap>

#include <utility>

#include "AccountManager.h"
#include "Chat.h"
#include "ChatManager.h"
#include "RequestManager.h"
#include "UserResolver.h"

ChatCreator& ChatCreator::instance()
{
    static ChatCreator instance;
    return instance;
}

ChatCreator::ChatCreator(QObject* parent)
    : QObject(parent),
      m_creating(false)
{
    const UserResolver& userResolver = UserResolver::instance();
    const RequestManager& requestManager = RequestManager::instance();

    connect(&userResolver, &UserResolver::userResolved, this, &ChatCreator::onUserResolved);
    connect(&requestManager, &RequestManager::operationResultReceived, this, &ChatCreator::onOperationResultReceived);
    connect(&requestManager, &RequestManager::chatInfoReceived, this, &ChatCreator::onChatInfoReceived);
}

void ChatCreator::addUser(const QString& userIdText)
{
    if (m_pendingUserId.has_value())
        return;

    const QUuid userId(userIdText.trimmed());
    const std::optional<QUuid> currentUserId = AccountManager::instance().userId();

    if (userId.isNull() || (currentUserId.has_value() && userId == currentUserId.value()))
    {
        setStatusText("Invalid user ID");
        return;
    }

    if (m_memberIds.contains(userId))
    {
        setStatusText("User already added");
        return;
    }

    const auto userInfo = UserResolver::instance().resolveUser(userId);
    if (userInfo.has_value())
    {
        addResolvedUser(userInfo.value());
        return;
    }

    m_pendingUserId = userId;
    setStatusText("Loading...");
    emit membersChanged();
    emitCreateStateChanged();
}

void ChatCreator::createChat()
{
    if (!canCreateChat())
        return;

    for (const auto& chat : ChatManager::instance().chats())
    {
        if (chat->otherMembers() == m_memberIds)
        {
            setStatusText("This chat already exists");
            return;
        }
    }

    QList<QUuid> memberIds = m_memberIds.values();
    m_creating = true;
    emitCreateStateChanged();
    setStatusText("Creating chat...");

    RequestManager::instance().createChat(std::move(memberIds));
}

void ChatCreator::reset()
{
    m_members.clear();
    m_memberIds.clear();
    m_pendingUserId.reset();
    m_creating = false;
    setStatusText("");
    emit membersChanged();
    emitCreateStateChanged();
}

QVariantList ChatCreator::members() const
{
    QVariantList members;

    for (const auto& userInfo : m_members)
    {
        QVariantMap member;
        member["userId"] = userInfo.userId().toString(QUuid::WithoutBraces);
        member["username"] = userInfo.username();
        members.append(member);
    }

    if (m_pendingUserId.has_value())
    {
        QVariantMap pendingMember;
        pendingMember["userId"] = m_pendingUserId->toString(QUuid::WithoutBraces);
        pendingMember["username"] = "Loading...";
        members.append(pendingMember);
    }

    return members;
}

void ChatCreator::addResolvedUser(const shared::PublicUserInfo& userInfo)
{
    if (userInfo.userId().isNull() || m_memberIds.contains(userInfo.userId()))
        return;

    m_members.append(userInfo);
    m_memberIds.insert(userInfo.userId());
    setStatusText("Successfully added " + userInfo.username());
    emit membersChanged();
    emitCreateStateChanged();
}

void ChatCreator::clearPendingUser()
{
    if (!m_pendingUserId.has_value())
        return;

    m_pendingUserId.reset();
    emit membersChanged();
    emitCreateStateChanged();
}

void ChatCreator::setStatusText(QString statusText)
{
    if (m_statusText == statusText)
        return;

    m_statusText = std::move(statusText);
    emit statusTextChanged();
}

void ChatCreator::emitCreateStateChanged()
{
    emit resolvingChanged();
    emit canCreateChatChanged();
}

void ChatCreator::onUserResolved(const QUuid& userId, const shared::PublicUserInfo& userInfo)
{
    if (!m_pendingUserId.has_value() || userId != m_pendingUserId.value())
        return;

    clearPendingUser();
    addResolvedUser(userInfo);
}

void ChatCreator::onOperationResultReceived(const shared::OperationResult& result)
{
    if (m_pendingUserId.has_value() && result.type() == shared::OperationResultType::ERROR) {
        clearPendingUser();
        setStatusText("Invalid user ID");
        return;
    }

    if (m_creating && result.type() == shared::OperationResultType::ERROR) {
        m_creating = false;
        emitCreateStateChanged();
        setStatusText(result.text());
    }
}

void ChatCreator::onChatInfoReceived(const shared::ChatInfo& chatInfo)
{
    if (!m_creating)
        return;

    QSet<QUuid> memberIds;
    for (const QUuid& memberId : chatInfo.memberIds())
        memberIds.insert(memberId);

    if (const std::optional<QUuid> currentUserId = AccountManager::instance().userId(); currentUserId.has_value())
        memberIds.remove(currentUserId.value());

    ChatManager::instance().addChat(new Chat(chatInfo.id(), std::move(memberIds), &ChatManager::instance()));
    ChatManager::instance().selectChat(chatInfo.id());

    reset();
}
