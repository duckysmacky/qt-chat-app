#pragma once

#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QUuid>

namespace model {

class User
{
public:
    User();
    User(const QString& username,
         const QString& name,
         const QString& password,
         const QString& email);

    const QUuid& id() const;
    const QString& username() const;
    const QString& name() const;
    const QString& passwordHash() const;
    const QString& email() const;

    void setId(const QUuid& id);
    void setUsername(const QString& username);
    void setName(const QString& name);
    void setPassword(const QString& password);
    void setPasswordHash(const QString& passwordHash);
    void setEmail(const QString& email);

private:
    static QString hashPassword(const QString& password);

private:
    QUuid m_id;
    QString m_username;
    QString m_name;
    QString m_passwordHash;
    QString m_email;
};

class Content
{
public:
    Content();
    Content(const QString& content,
            const QString& file,
            double fileSize);

    const QUuid& id() const;
    const QString& content() const;
    const QString& file() const;
    double fileSize() const;

    void setId(const QUuid& id);
    void setContent(const QString& content);
    void setFile(const QString& file);
    void setFileSize(double fileSize);

private:
    QUuid m_id;
    QString m_content;
    QString m_file;
    double m_fileSize = 0.0;
};

class Chat
{
public:
    Chat();
    Chat(const QString& type,
         const QUuid& createdBy,
         const QString& title);

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

class ChatMember
{
public:
    ChatMember();
    ChatMember(const QUuid& chatId,
               const QUuid& userId);

    const QUuid& chatId() const;
    const QUuid& userId() const;

    void setChatId(const QUuid& chatId);
    void setUserId(const QUuid& userId);

private:
    QUuid m_chatId;
    QUuid m_userId;
};

class DbMessage
{
public:
    DbMessage();
    DbMessage(const QUuid& toId,
              const QUuid& fromId,
              const QUuid& contentId,
              const QUuid& chatId);

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

class UserStats
{
public:
    UserStats();
    UserStats(const QUuid& userId,
              int messagesSent = 0,
              int chatsJoined = 0,
              const QStringList& exUsernames = {});

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
