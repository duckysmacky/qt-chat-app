#pragma once

#include <QObject>
#include <QTime>
#include <QString>
#include <QUuid>

#include <optional>

/**
 * @class ChatMessage
 * @brief Represents a single chat message with delivery status and timestamps.
 *        Exposes properties for QML integration.
 */
class ChatMessage : public QObject
{
    /**
     * @brief Q_PROPERTY definitions for the ChatMessage class.
     * 
     * These properties expose the message attributes to the QML engine.
     */

    Q_OBJECT
        /**
         * @property isOwn
         * @brief Indicates whether the current user is the sender of this message.
         * 
         * Returns true if the message was sent by the currently authenticated user,
         * false otherwise. This property is constant and does not change over time.
         */
        Q_PROPERTY(bool isOwn READ isOwn CONSTANT)

        /**
         * @property content
         * @brief The text content of the chat message.
         * 
         * Contains the actual message text sent by the user. This property is
         * constant and immutable after message creation.
         */
        Q_PROPERTY(QString content READ content CONSTANT)

        /**
         * @property sender
         * @brief The display name or identifier of the message sender.
         * 
         * Returns the name of the user who sent this message. This property
         * emits the senderChanged signal when updated.
         */
        Q_PROPERTY(QString sender READ sender NOTIFY senderChanged)

        /**
         * @property timeSent
         * @brief The formatted timestamp when the message was sent.
         * 
         * Returns a human-readable formatted string representing the time
         * the message was sent. Emits timeSentChanged signal when updated.
         */
        Q_PROPERTY(QString timeSent READ formattedTimeSent NOTIFY timeSentChanged)

        /**
         * @property timeReceived
         * @brief The formatted timestamp when the message was received.
         * 
         * Returns a human-readable formatted string representing the time
         * the message was received by the client. Emits timeReceivedChanged
         * signal when updated.
         */
        Q_PROPERTY(QString timeReceived READ formattedTimeReceived NOTIFY timeReceivedChanged)

        /**
         * @property status
         * @brief The current delivery status of the message.
         * 
         * Indicates the message's delivery state (e.g., sending, sent,
         * delivered, read, failed). Emits statusChanged signal when updated.
         */
        Q_PROPERTY(Status status READ status NOTIFY statusChanged)

        /**
         * @property statusText
         * @brief A human-readable description of the message status.
         * 
         * Provides user-friendly text describing the current message status
         * (e.g., "Sending...", "Sent", "Delivered", "Failed to send").
         * Emits statusChanged signal when updated.
         */
        Q_PROPERTY(QString statusText READ statusText NOTIFY statusChanged)
public:
    /**
     * @enum Status
     * @brief Represents the delivery status of a message.
     */
    enum class Status
    {
        Loading,   ///< Message is being sent/loaded
        Sent,      ///< Message has been sent to the server
        Delivered, ///< Message has been delivered to the recipient's device
        Read,      ///< Message has been read by the recipient
        Failed,
        Received   ///< Message has been received by the recipient
    };
    Q_ENUM(Status)

private:
    const QUuid m_id;                ///< Unique identifier of the message
    const bool m_isOwn;              ///< Whether the message was sent by the current user
    const QString m_content;         ///< Text content of the message
    const QUuid m_senderUserId;      ///< Unique identifier of the message sender
    QString m_sender;                ///< Username of the message sender
    std::optional<QTime> m_timeSent;     ///< Timestamp when the message was sent
    std::optional<QTime> m_timeReceived; ///< Timestamp when the message was received
    Status m_status;                 ///< Current delivery status of the message

public:
    /**
     * @brief Constructs a ChatMessage with a specific ID.
     * @param id The UUID of the message.
     * @param isOwn True if the message is from the current user.
     * @param content The message text content.
     * @param senderUserId The UUID of the sender.
     * @param parent Parent QObject (default nullptr).
     */
    ChatMessage(const QUuid& id, bool isOwn, QString content, const QUuid& senderUserId, QObject* parent = nullptr);

    /**
     * @brief Constructs a ChatMessage with an auto-generated ID.
     * @param isOwn True if the message is from the current user.
     * @param content The message text content.
     * @param senderUserId The UUID of the sender.
     * @param parent Parent QObject (default nullptr).
     */
    ChatMessage(bool isOwn, QString content, const QUuid& senderUserId, QObject* parent = nullptr);

    /// @brief Marks the message as sent and updates the timestamp.
    void markAsSent();

    /// @brief Marks the message as received and updates the timestamp.
    void markAsReceived();

    /**
     * @brief Checks if the message has been sent.
     * @return True if the message has been sent, false otherwise.
     */
    Q_INVOKABLE bool isSent() const;

    /**
     * @brief Checks if the message has been received.
     * @return True if the message has been received, false otherwise.
     */
    Q_INVOKABLE bool isReceived() const;

    /// @brief Returns the formatted time the message was sent.
    /// @return Formatted time string or empty string if not sent.
    QString formattedTimeSent() const;

    /// @brief Returns the formatted time the message was received.
    /// @return Formatted time string or empty string if not received.
    QString formattedTimeReceived() const;

    /// @brief Returns a human-readable status text.
    /// @return Status string (e.g., "Sent", "Delivered", "Read").
    QString statusText() const;

    /// @brief Sets the sent timestamp.
    /// @param time The time when the message was sent.
    void setTimeSent(const QTime& time);

    /// @brief Sets the received timestamp.
    /// @param time The time when the message was received.
    void setTimeReceived(const QTime& time);

    /// @brief Sets the delivery status of the message.
    /// @param status The new status.
    void setStatus(Status status);

    /// @brief Returns the unique ID of the message.
    const QUuid& id() const { return m_id; }

    /// @brief Returns whether the message is from the current user.
    bool isOwn() const { return m_isOwn; }

    /// @brief Returns the message content.
    const QString& content() const { return m_content; }

    /// @brief Returns the sender's username.
    const QString& sender() const { return m_sender; }

    /// @brief Returns the optional sent timestamp.
    const std::optional<QTime>& timeSent() const { return m_timeSent; }

    /// @brief Returns the optional received timestamp.
    const std::optional<QTime>& timeReceived() const { return m_timeReceived; }

    /// @brief Returns the current delivery status.
    Status status() const { return m_status; }

signals:
    void statusChanged();      ///< Emitted when the message status changes.
    void senderChanged();      ///< Emitted when the resolved sender username changes.
    void timeSentChanged();    ///< Emitted when the sent timestamp changes.
    void timeReceivedChanged(); ///< Emitted when the received timestamp changes.

private:
    void resolveSender();
    void setSender(QString sender);

    /**
     * @brief Formats a QTime object into a string (HH:MM:SS).
     * @param time The time to format.
     * @return Formatted time string.
     */
    QString formatTime(const QTime& time) const;
};
