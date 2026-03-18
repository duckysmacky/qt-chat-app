#include "model.h"

#include <QCryptographicHash>

namespace model {

User::User() = default;

User::User(const QString& username,
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

const QUuid& User::id() const
{
    return m_id;
}

const QString& User::username() const
{
    return m_username;
}

const QString& User::name() const
{
    return m_name;
}

const QString& User::passwordHash() const
{
    return m_passwordHash;
}

const QString& User::email() const
{
    return m_email;
}

void User::setId(const QUuid& id)
{
    m_id = id;
}

void User::setUsername(const QString& username)
{
    m_username = username;
}

void User::setName(const QString& name)
{
    m_name = name;
}

void User::setPassword(const QString& password)
{
    m_passwordHash = hashPassword(password);
}

void User::setPasswordHash(const QString& passwordHash)
{
    m_passwordHash = passwordHash;
}

void User::setEmail(const QString& email)
{
    m_email = email;
}

QString User::hashPassword(const QString& password)
{
    return QString(
        QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex()
        );
}

Content::Content() = default;

Content::Content(const QString& content,
                 const QString& file,
                 double fileSize)
    : m_id(QUuid::createUuid()),
    m_content(content),
    m_file(file),
    m_fileSize(fileSize)
{
}

const QUuid& Content::id() const
{
    return m_id;
}

const QString& Content::content() const
{
    return m_content;
}

const QString& Content::file() const
{
    return m_file;
}

double Content::fileSize() const
{
    return m_fileSize;
}

void Content::setId(const QUuid& id)
{
    m_id = id;
}

void Content::setContent(const QString& content)
{
    m_content = content;
}

void Content::setFile(const QString& file)
{
    m_file = file;
}

void Content::setFileSize(double fileSize)
{
    m_fileSize = fileSize;
}

Chat::Chat() = default;

Chat::Chat(const QString& type,
           const QUuid& createdBy,
           const QString& title)
    : m_id(QUuid::createUuid()),
    m_type(type),
    m_createdBy(createdBy),
    m_title(title),
    m_createdAt(QDateTime::currentDateTime())
{
}

const QUuid& Chat::id() const
{
    return m_id;
}

const QString& Chat::type() const
{
    return m_type;
}

const QUuid& Chat::createdBy() const
{
    return m_createdBy;
}

const QString& Chat::title() const
{
    return m_title;
}

const QDateTime& Chat::createdAt() const
{
    return m_createdAt;
}

void Chat::setId(const QUuid& id)
{
    m_id = id;
}

void Chat::setType(const QString& type)
{
    m_type = type;
}

void Chat::setCreatedBy(const QUuid& createdBy)
{
    m_createdBy = createdBy;
}

void Chat::setTitle(const QString& title)
{
    m_title = title;
}

void Chat::setCreatedAt(const QDateTime& createdAt)
{
    m_createdAt = createdAt;
}

ChatMember::ChatMember() = default;

ChatMember::ChatMember(const QUuid& chatId,
                       const QUuid& userId)
    : m_chatId(chatId),
    m_userId(userId)
{
}

const QUuid& ChatMember::chatId() const
{
    return m_chatId;
}

const QUuid& ChatMember::userId() const
{
    return m_userId;
}

void ChatMember::setChatId(const QUuid& chatId)
{
    m_chatId = chatId;
}

void ChatMember::setUserId(const QUuid& userId)
{
    m_userId = userId;
}

DbMessage::DbMessage() = default;

DbMessage::DbMessage(const QUuid& toId,
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

const QUuid& DbMessage::id() const
{
    return m_id;
}

const QUuid& DbMessage::toId() const
{
    return m_toId;
}

const QUuid& DbMessage::fromId() const
{
    return m_fromId;
}

const QUuid& DbMessage::contentId() const
{
    return m_contentId;
}

const QUuid& DbMessage::chatId() const
{
    return m_chatId;
}

const QDateTime& DbMessage::createdAt() const
{
    return m_createdAt;
}

const QDateTime& DbMessage::updatedAt() const
{
    return m_updatedAt;
}

void DbMessage::setId(const QUuid& id)
{
    m_id = id;
}

void DbMessage::setToId(const QUuid& toId)
{
    m_toId = toId;
}

void DbMessage::setFromId(const QUuid& fromId)
{
    m_fromId = fromId;
}

void DbMessage::setContentId(const QUuid& contentId)
{
    m_contentId = contentId;
}

void DbMessage::setChatId(const QUuid& chatId)
{
    m_chatId = chatId;
}

void DbMessage::setCreatedAt(const QDateTime& createdAt)
{
    m_createdAt = createdAt;
}

void DbMessage::setUpdatedAt(const QDateTime& updatedAt)
{
    m_updatedAt = updatedAt;
}

UserStats::UserStats() = default;

UserStats::UserStats(const QUuid& userId,
                     int messagesSent,
                     int chatsJoined,
                     const QStringList& exUsernames)
    : m_userId(userId),
    m_messagesSent(messagesSent),
    m_chatsJoined(chatsJoined),
    m_exUsernames(exUsernames)
{
}

const QUuid& UserStats::userId() const
{
    return m_userId;
}

int UserStats::messagesSent() const
{
    return m_messagesSent;
}

int UserStats::chatsJoined() const
{
    return m_chatsJoined;
}

const QStringList& UserStats::exUsernames() const
{
    return m_exUsernames;
}

void UserStats::setUserId(const QUuid& userId)
{
    m_userId = userId;
}

void UserStats::setMessagesSent(int messagesSent)
{
    m_messagesSent = messagesSent;
}

void UserStats::setChatsJoined(int chatsJoined)
{
    m_chatsJoined = chatsJoined;
}

void UserStats::setExUsernames(const QStringList& exUsernames)
{
    m_exUsernames = exUsernames;
}

}
