#pragma once

#include <QStringList>
#include <QUuid>

namespace model {

class UserStats
{
public:
    UserStats();
    UserStats(QUuid userId,
              int messagesSent = 0,
              int chatsJoined = 0,
              QStringList exUsernames = {});

    const QUuid& userId() const;
    int messagesSent() const;
    int chatsJoined() const;
    const QStringList& exUsernames() const;

    void setUserId(const QUuid& userId);
    void setMessagesSent(int messagesSent);
    void setChatsJoined(int chatsJoined);
    void setExUsernames(const QStringList& exUsernames);

private:
    QUuid m_userId;
    int m_messagesSent = 0;
    int m_chatsJoined = 0;
    QStringList m_exUsernames;
};

}
