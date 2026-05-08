#pragma once

#include <QByteArray>
#include <QString>
#include <QUuid>

#include <optional>

namespace shared {

enum class UserIdentifierType
{
    UUID,
    USERNAME
};

class UserInfoRequest
{
public:
    UserInfoRequest();
    explicit UserInfoRequest(QUuid userId);
    explicit UserInfoRequest(QString username);

    UserIdentifierType identifierType() const { return m_identifierType; }
    const QUuid& userId() const { return m_userId; }
    const QString& username() const { return m_username; }

    QByteArray serialize() const;
    static std::optional<UserInfoRequest> deserialize(const QByteArray& bytes);

private:
    UserIdentifierType m_identifierType;
    QUuid m_userId;
    QString m_username;
};

}
