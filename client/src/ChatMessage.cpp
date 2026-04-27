#include "ChatMessage.h"

#include <QTime>

ChatMessage::ChatMessage(
    const QUuid& id,
    const bool isOwn,
    QString content,
    QString sender,
    QObject* parent
)
  : QObject(parent),
    m_id(id),
    m_isOwn(isOwn),
    m_content(std::move(content)),
    m_sender(std::move(sender)),
    m_status(Status::Loading)
{
}

ChatMessage::ChatMessage(
    const bool isOwn,
    QString content,
    QString sender,
    QObject* parent
)
  : QObject(parent),
    m_id(QUuid::createUuid()),
    m_isOwn(isOwn),
    m_content(std::move(content)),
    m_sender(std::move(sender)),
    m_status(Status::Loading)
{
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

QString ChatMessage::formatTime(const QTime& time) const
{
    return time.toString("hh:mm");
}
