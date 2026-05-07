#pragma once

#include <QList>
#include <QObject>
#include <QSet>
#include <QString>
#include <QUuid>
#include <QVariantList>

#include <optional>

#include "OperationResult.h"
#include "dto/ChatInfo.h"
#include "dto/PublicUserInfo.h"

class ChatCreator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList members READ members NOTIFY membersChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(bool canCreateChat READ canCreateChat NOTIFY canCreateChatChanged)
    Q_PROPERTY(bool resolving READ resolving NOTIFY resolvingChanged)

private:
    QList<shared::PublicUserInfo> m_members;
    QSet<QUuid> m_memberIds;
    std::optional<QString> m_pendingUsername;
    QString m_statusText;
    bool m_creating;

public:
    static ChatCreator& instance();

    ChatCreator(const ChatCreator&) = delete;
    ChatCreator& operator=(const ChatCreator&) = delete;
    ChatCreator(ChatCreator&&) = delete;
    ChatCreator& operator=(ChatCreator&&) = delete;

    Q_INVOKABLE void addUser(const QString& usernameText);
    Q_INVOKABLE void createChat();
    Q_INVOKABLE void reset();

    QVariantList members() const;
    const QString& statusText() const { return m_statusText; }
    bool canCreateChat() const { return !m_memberIds.isEmpty() && !resolving() && !m_creating; }
    bool resolving() const { return m_pendingUsername.has_value(); }

signals:
    void membersChanged();
    void statusTextChanged();
    void canCreateChatChanged();
    void resolvingChanged();

private:
    explicit ChatCreator(QObject* parent = nullptr);

    void addResolvedUser(const shared::PublicUserInfo& userInfo);
    void clearPendingUser();
    void setStatusText(QString statusText);
    void emitCreateStateChanged();

    void onUserResolved(const QUuid& userId, const shared::PublicUserInfo& userInfo);
    void onOperationResultReceived(const shared::OperationResult& result);
    void onChatInfoReceived(const shared::ChatInfo& chatInfo);
};
