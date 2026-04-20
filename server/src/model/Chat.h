#pragma once

#include <QDateTime>
#include <QString>
#include <QUuid>

namespace model {

class Chat
{
public:
    Chat();
    Chat(QString type,
         QUuid createdBy,
         QString title);

    const QUuid& id() const;
    const QString& type() const;
    const QUuid& createdBy() const;
    const QString& title() const;
    const QDateTime& createdAt() const;

    void setId(const QUuid& id);
    void setType(const QString& type);
    void setCreatedBy(const QUuid& createdBy);
    void setTitle(const QString& title);
    void setCreatedAt(const QDateTime& createdAt);

private:
    QUuid m_id;
    QString m_type;
    QUuid m_createdBy;
    QString m_title;
    QDateTime m_createdAt;
};

}
