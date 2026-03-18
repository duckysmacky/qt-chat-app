#include <QUuid>
#include <QSqlDatabase>
#include <QString>
#include <QCryptographicHash>
#include <QDateTime>
namespace model{


class User
{
public:
    User() = default;

    User(const QString& username,
         const QString& name,
         const QString& password,
         const QString& email)
        : m_id(QUuid::createUuid()),
        m_username(username),
        m_name(name),
        m_passwordHash(hashPassword(password)),
        m_email(email)
    {
    }

    const QUuid& id() const
    {
        return m_id;
    }

    const QString& username() const
    {
        return m_username;
    }

    const QString& name() const
    {
        return m_name;
    }

    const QString& passwordHash() const
    {
        return m_passwordHash;
    }

    const QString& email() const
    {
        return m_email;
    }

    void setId(const QUuid& id)
    {
        m_id = id;
    }

    void setUsername(const QString& username)
    {
        m_username = username;
    }

    void setName(const QString& name)
    {
        m_name = name;
    }

    void setPassword(const QString& password)
    {
        m_passwordHash = hashPassword(password);
    }

    void setPasswordHash(const QString& passwordHash)
    {
        m_passwordHash = passwordHash;
    }

    void setEmail(const QString& email)
    {
        m_email = email;
    }

private:
    static QString hashPassword(const QString& password)
    {
        return QString(
            QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex()
            );
    }

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
    Content() = default;

    Content(const QString& content,
            const QString& file,
            double fileSize)
        : m_id(QUuid::createUuid()),
        m_content(content),
        m_file(file),
        m_fileSize(fileSize)
    {
    }

    const QUuid& id() const
    {
        return m_id;
    }

    const QString& content() const
    {
        return m_content;
    }

    const QString& file() const
    {
        return m_file;
    }

    double fileSize() const
    {
        return m_fileSize;
    }

    void setId(const QUuid& id)
    {
        m_id = id;
    }

    void setContent(const QString& content)
    {
        m_content = content;
    }

    void setFile(const QString& file)
    {
        m_file = file;
    }

    void setFileSize(double fileSize)
    {
        m_fileSize = fileSize;
    }

private:
    QUuid m_id;
    QString m_content;
    QString m_file;
    double m_fileSize = 0.0;
};

class Chat
{
public:
    Chat() = default;

    Chat(const QString& type,
         const QUuid& createdBy,
         const QString& title)
        : m_id(QUuid::createUuid()),
        m_type(type),
        m_createdBy(createdBy),
        m_title(title),
        m_createdAt(QDateTime::currentDateTime())
    {
    }

    const QUuid& id() const
    {
        return m_id;
    }

    const QString& type() const
    {
        return m_type;
    }

    const QUuid& createdBy() const
    {
        return m_createdBy;
    }

    const QString& title() const
    {
        return m_title;
    }

    const QDateTime& createdAt() const
    {
        return m_createdAt;
    }

    void setId(const QUuid& id)
    {
        m_id = id;
    }

    void setType(const QString& type)
    {
        m_type = type;
    }

    void setCreatedBy(const QUuid& createdBy)
    {
        m_createdBy = createdBy;
    }

    void setTitle(const QString& title)
    {
        m_title = title;
    }

    void setCreatedAt(const QDateTime& createdAt)
    {
        m_createdAt = createdAt;
    }

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
    ChatMember() = default;

    ChatMember(const QUuid& chatId,
               const QUuid& userId)
        : m_chatId(chatId),
        m_userId(userId)
    {
    }

    const QUuid& chatId() const
    {
        return m_chatId;
    }

    const QUuid& userId() const
    {
        return m_userId;
    }

    void setChatId(const QUuid& chatId)
    {
        m_chatId = chatId;
    }

    void setUserId(const QUuid& userId)
    {
        m_userId = userId;
    }

private:
    QUuid m_chatId;
    QUuid m_userId;
};

class DbMessage
{
public:
    DbMessage() = default;

    DbMessage(const QUuid& toId,
              const QUuid& fromId,
              const QUuid& contentId,
              const QUuid& chatId)
        : m_id(QUuid::createUuid()),
        m_toId(toId),
        m_fromId(fromId),
        m_contentId(contentId),
        m_chatId(chatId),
        m_createdAt(QDateTime::currentDateTime())
    {
    }

    const QUuid& id() const
    {
        return m_id;
    }

    const QUuid& toId() const
    {
        return m_toId;
    }

    const QUuid& fromId() const
    {
        return m_fromId;
    }

    const QUuid& contentId() const
    {
        return m_contentId;
    }

    const QUuid& chatId() const
    {
        return m_chatId;
    }

    const QDateTime& createdAt() const
    {
        return m_createdAt;
    }

    const QDateTime& updatedAt() const
    {
        return m_updatedAt;
    }

    void setId(const QUuid& id)
    {
        m_id = id;
    }

    void setToId(const QUuid& toId)
    {
        m_toId = toId;
    }

    void setFromId(const QUuid& fromId)
    {
        m_fromId = fromId;
    }

    void setContentId(const QUuid& contentId)
    {
        m_contentId = contentId;
    }

    void setChatId(const QUuid& chatId)
    {
        m_chatId = chatId;
    }

    void setCreatedAt(const QDateTime& createdAt)
    {
        m_createdAt = createdAt;
    }

    void setUpdatedAt(const QDateTime& updatedAt)
    {
        m_updatedAt = updatedAt;
    }

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
    UserStats() = default;

    UserStats(const QUuid& userId,
              int messagesSent = 0,
              int chatsJoined = 0,
              const QStringList& exUsernames = {})
        : m_userId(userId),
        m_messagesSent(messagesSent),
        m_chatsJoined(chatsJoined),
        m_exUsernames(exUsernames)
    {
    }

    const QUuid& userId() const
    {
        return m_userId;
    }

    int messagesSent() const
    {
        return m_messagesSent;
    }

    int chatsJoined() const
    {
        return m_chatsJoined;
    }

    const QStringList& exUsernames() const
    {
        return m_exUsernames;
    }

    void setUserId(const QUuid& userId)
    {
        m_userId = userId;
    }

    void setMessagesSent(int messagesSent)
    {
        m_messagesSent = messagesSent;
    }

    void setChatsJoined(int chatsJoined)
    {
        m_chatsJoined = chatsJoined;
    }

    void setExUsernames(const QStringList& exUsernames)
    {
        m_exUsernames = exUsernames;
    }

private:
    QUuid m_userId;
    int m_messagesSent = 0;
    int m_chatsJoined = 0;
    QStringList m_exUsernames;
};

}
