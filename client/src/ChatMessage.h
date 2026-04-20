#pragma once

#include <QObject>
#include <QTime>
#include <QString>
#include <QUuid>

#include <optional>

/**
 * @brief UI chat message model.
 * Represents a single message with status and timestamps.
 */
class ChatMessage : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isOwn READ isOwn CONSTANT)
    Q_PROPERTY(QString content READ content CONSTANT)
    Q_PROPERTY(QString sender READ sender CONSTANT)
    Q_PROPERTY(QString timeSent READ formattedTimeSent NOTIFY timeSentChanged)
    Q_PROPERTY(QString timeReceived READ formattedTimeReceived NOTIFY timeReceivedChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusChanged)

public:
    /**
     * @brief Message delivery state.
     */
    enum class Status
    {
        Loading,
        Sent,
        Delivered,
        Read,
        Received
    };

    Q_ENUM(Status)

private:
    const QUuid m_id;
    const bool m_isOwn;
    const QString m_content;
    const QString m_sender;

    std::optional<QTime> m_timeSent;
    std::optional<QTime> m_timeReceived;
    Status m_status;

public:
    ChatMessage(const QUuid& id, bool isOwn, QString content, QString sender, QObject* parent = nullptr);
    ChatMessage(bool isOwn, QString content, QString sender, QObject* parent = nullptr);

    /// Marks message as sent.
    void markAsSent();

    /// Marks message as received.
    void markAsReceived();

    Q_INVOKABLE bool isSent() const;
    Q_INVOKABLE bool isReceived() const;

    QString formattedTimeSent() const;
    QString formattedTimeReceived() const;
    QString statusText() const;

    void setTimeSent(const QTime& time);
    void setTimeReceived(const QTime& time);
    void setStatus(Status status);

    const QUuid& id() const { return m_id; }
    bool isOwn() const { return m_isOwn; }
    const QString& content() const { return m_content; }
    const QString& sender() const { return m_sender; }
    const std::optional<QTime>& timeSent() const { return m_timeSent; }
    const std::optional<QTime>& timeReceived() const { return m_timeReceived; }
    Status status() const { return m_status; }

signals:
    void statusChanged();
    void timeSentChanged();
    void timeReceivedChanged();

private:
    /// Formats QTime to hh:mm string.
    QString formatTime(const QTime& time) const;
};