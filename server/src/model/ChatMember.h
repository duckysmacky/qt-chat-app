#pragma once

#include <QUuid>

namespace model {

class ChatMember
{
public:
    ChatMember();
    ChatMember(QUuid chatId,
               QUuid userId);

    const QUuid& chatId() const;
    const QUuid& userId() const;

    void setChatId(const QUuid& chatId);
    void setUserId(const QUuid& userId);

private:
    QUuid m_chatId;
    QUuid m_userId;
};

}
