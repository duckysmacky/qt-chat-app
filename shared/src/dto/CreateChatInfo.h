#pragma once

#include <QByteArray>
#include <QString>

#include <optional>

namespace shared {

class ChatCreateInfo
{
public:
    ChatCreateInfo();
    ChatCreateInfo(QString type, QString title);

    const QString& type() const { return m_type; }
    const QString& title() const { return m_title; }

    QByteArray serialize() const;
    static std::optional<ChatCreateInfo> deserialize(const QByteArray& bytes);

private:
    QString m_type;
    QString m_title;
};

}
