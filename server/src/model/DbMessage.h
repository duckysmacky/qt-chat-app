#pragma once

#include <QDateTime>
#include <QUuid>

namespace model {

class DbMessage
{
public:
    DbMessage();
    DbMessage(QUuid toId,
              QUuid fromId,
              QUuid contentId,
              QUuid chatId);

    const QUuid& id() const;
    const QUuid& toId() const;
    const QUuid& fromId() const;
    const QUuid& contentId() const;
    const QUuid& chatId() const;
    const QDateTime& createdAt() const;
    const QDateTime& updatedAt() const;

    void setId(const QUuid& id);
    void setToId(const QUuid& toId);
    void setFromId(const QUuid& fromId);
    void setContentId(const QUuid& contentId);
    void setChatId(const QUuid& chatId);
    void setCreatedAt(const QDateTime& createdAt);
    void setUpdatedAt(const QDateTime& updatedAt);

private:
    QUuid m_id;
    QUuid m_toId;
    QUuid m_fromId;
    QUuid m_contentId;
    QUuid m_chatId;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
};

}
