#pragma once

#include <QString>
#include <QUuid>

namespace model {

class User
{
public:
    User();
    User(QString username,
         QString name,
         QString passwordHash,
         QString email);

    const QUuid& id() const;
    const QString& username() const;
    const QString& name() const;
    const QString& passwordHash() const;
    const QString& email() const;

    void setId(const QUuid& id);
    void setUsername(const QString& username);
    void setName(const QString& name);
    void setPasswordHash(const QString& passwordHash);
    void setEmail(const QString& email);

private:
    QUuid m_id;
    QString m_username;
    QString m_name;
    QString m_passwordHash;
    QString m_email;
};

}
