#include "ChatMessage.h"

#include <QTime>

#include <utility>

#include "AccountManager.h"
#include "UserResolver.h"

ChatMessage::ChatMessage(
    const QUuid& id,
    const bool isOwn,
    QString content,
    const QUuid& senderUserId,
    QObject* parent
)
  : QObject(parent),
    m_id(id),
    m_isOwn(isOwn),
    m_content(std::move(content)),
    m_senderUserId(senderUserId),
    m_status(Status::Loading)
{
    resolveSender();
}

ChatMessage::ChatMessage(
    const bool isOwn,
    QString content,
    const QUuid& senderUserId,
    QObject* parent
)
  : QObject(parent),
    m_id(QUuid::createUuid()),
    m_isOwn(isOwn),
    m_content(std::move(content)),
    m_senderUserId(senderUserId),
    m_status(Status::Loading)
{
    resolveSender();
}

void ChatMessage::markAsSent()
{
    setStatus(Status::Sent);
    setTimeSent(QTime::currentTime());
}

void ChatMessage::markAsReceived()
{
    setStatus(Status::Received);
    setTimeReceived(QTime::currentTime());
}

bool ChatMessage::isSent() const
{
    return m_isOwn
        && static_cast<int>(m_status) >= static_cast<int>(Status::Sent)
        && m_timeSent.has_value();
}

bool ChatMessage::isReceived() const
{
    return !m_isOwn
        && m_status == Status::Received
        && m_timeReceived.has_value();
}

QString ChatMessage::formattedTimeSent() const
{
    return m_timeSent.has_value() ? formatTime(m_timeSent.value()) : "";
}

QString ChatMessage::formattedTimeReceived() const
{
    return m_timeReceived.has_value() ? formatTime(m_timeReceived.value()) : "";
}

QString ChatMessage::statusText() const
{
    switch (m_status)
    {
    case Status::Loading:
        return "Sending";
    case Status::Sent:
        return "Sent";
    case Status::Delivered:
        return "Delivered";
    case Status::Read:
        return "Read";
    case Status::Failed:
        return "Failed";
    case Status::Received:
        return "Received";
    default:
        return "";
    }
}

void ChatMessage::setTimeSent(const QTime& time)
{
    if (m_timeSent == time) return;

    m_timeSent = time;
    emit timeSentChanged();
}

void ChatMessage::setTimeReceived(const QTime& time)
{
    if (m_timeReceived == time) return;

    m_timeReceived = time;
    emit timeReceivedChanged();
}

void ChatMessage::setStatus(const Status status)
{
    if (m_status == status) return;

    m_status = status;
    emit statusChanged();
}

void ChatMessage::resolveSender()
{
    if (m_isOwn) {
        setSender(AccountManager::instance().profileUsername());
        return;
    }

    if (m_senderUserId.isNull()) {
        setSender("Unknown");
        return;
    }

    setSender("Loading...");

    if (const auto userInfo = UserResolver::instance().resolveUser(m_senderUserId); userInfo.has_value()) {
        setSender(userInfo->username());
        return;
    }

    connect(
        &UserResolver::instance(),
        &UserResolver::userResolved,
        this,
        [this](const QUuid& userId, const shared::PublicUserInfo& userInfo) {
            if (userId != m_senderUserId)
                return;

            setSender(userInfo.username());
        }
    );
}

void ChatMessage::setSender(QString sender)
{
    if (m_sender == sender) return;

    m_sender = std::move(sender);
    emit senderChanged();
}

QString ChatMessage::formatTime(const QTime& time) const
{
    return time.toString("hh:mm");
}
