#pragma once

#include <QByteArray>
#include <QDateTime>
#include <QString>
#include <QUuid>

#include <optional>

namespace shared {

class ChatInfo
{
public:
    ChatInfo();
    ChatInfo(QUuid id, QString type, QUuid createdBy, QString title, QDateTime createdAt);

    const QUuid& id() const { return m_id; }
    const QString& type() const { return m_type; }
    const QUuid& createdBy() const { return m_createdBy; }
    const QString& title() const { return m_title; }
    const QDateTime& createdAt() const { return m_createdAt; }

    QByteArray serialize() const;
    static std::optional<ChatInfo> deserialize(const QByteArray& bytes);

private:
    QUuid m_id;
    QString m_type;
    QUuid m_createdBy;
    QString m_title;
    QDateTime m_createdAt;
};

}
