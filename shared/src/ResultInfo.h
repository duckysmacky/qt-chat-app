#pragma once

#include <QByteArray>
#include <QString>

namespace shared {

enum class ResultType
{
    SUCCESS,
    ERROR
};

class ResultInfo
{
public:
    ResultInfo(ResultType type, QString text);

    static ResultInfo deserialize(QByteArray bytes);
    QByteArray serialize() const;

    ResultType type() const { return m_type; }
    const QString& text() const { return m_text; }

private:
    ResultType m_type;
    QString m_text;
};

}
