#include "ChatMessage.h"

#include <QTime>

/**
 * @brief Message in chat UI.
 * Stores content, sender, status and timestamps.
 */
ChatMessage::ChatMessage(
    const QUuid& id,
    bool isOwn,
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

/**
 * @brief Creates new message with generated id.
 */
ChatMessage::ChatMessage(
    bool isOwn,
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

/**
 * @brief Marks message as sent and sets send time.
 */
void ChatMessage::markAsSent()
{
    setStatus(Status::Sent);
    setTimeSent(QTime::currentTime());
}

/**
 * @brief Marks message as received and sets receive time.
 */
void ChatMessage::markAsReceived()
{
    setStatus(Status::Received);
    setTimeReceived(QTime::currentTime());
}

/**
 * @brief True if message was sent successfully.
 */
bool ChatMessage::isSent() const
{
    return m_isOwn
        && static_cast<int>(m_status) >= static_cast<int>(Status::Sent)
        && m_timeSent.has_value();
}

/**
 * @brief True if message was received.
 */
bool ChatMessage::isReceived() const
{
    return !m_isOwn
        && m_status == Status::Received
        && m_timeReceived.has_value();
}

/**
 * @brief Formatted send time (hh:mm).
 */
QString ChatMessage::formattedTimeSent() const
{
    return m_timeSent.has_value() ? formatTime(m_timeSent.value()) : "";
}

/**
 * @brief Formatted receive time (hh:mm).
 */
QString ChatMessage::formattedTimeReceived() const
{
    return m_timeReceived.has_value() ? formatTime(m_timeReceived.value()) : "";
}

/**
 * @brief Human-readable message status.
 */
QString ChatMessage::statusText() const
{
    switch (m_status)
    {
    case Status::Loading:   return "Sending";
    case Status::Sent:      return "Sent";
    case Status::Delivered: return "Delivered";
    case Status::Read:      return "Read";
    case Status::Received:  return "Received";
    }

    return "";
}

/**
 * @brief Sets send timestamp.
 */
void ChatMessage::setTimeSent(const QTime& time)
{
    if (m_timeSent == time) return;

    m_timeSent = time;
    emit timeSentChanged();
}

/**
 * @brief Sets receive timestamp.
 */
void ChatMessage::setTimeReceived(const QTime& time)
{
    if (m_timeReceived == time) return;

    m_timeReceived = time;
    emit timeReceivedChanged();
}

/**
 * @brief Updates message status.
 */
void ChatMessage::setStatus(const Status status)
{
    if (m_status == status) return;

    m_status = status;
    emit statusChanged();
}

/**
 * @brief Formats time as hh:mm string.
 */
QString ChatMessage::formatTime(const QTime& time) const
{
    return time.toString("hh:mm");
}