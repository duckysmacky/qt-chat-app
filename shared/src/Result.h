#pragma once

#include <QByteArray>
#include <QString>

namespace shared {

enum class ResultType
{
    SUCCESS,
    ERROR
};

class Result
{
private:
    ResultType m_type;
    QString m_text;

public:
    Result(ResultType type, QString text);

    Result(const Result& other) = default;
    Result& operator =(const Result& other) = default;
    Result(Result&& other) noexcept;
    Result& operator =(Result&& other) noexcept;

    ~Result() = default;

    static Result deserialize(QByteArray bytes);

    QByteArray serialize() const;

    ResultType type() const { return m_type; }
    const QString& text() const { return m_text; }
};

}
