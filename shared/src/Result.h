#pragma once

#include <QByteArray>
#include <QString>

namespace shared {

/**
 * @brief Type of operation result.
 */
enum class ResultType
{
    SUCCESS,
    ERROR
};

/**
 * @brief Simple result container with status and message.
 */
class Result
{
private:
    ResultType m_type;
    QString m_text;

public:
    /**
     * @brief Constructs result.
     */
    Result(ResultType type, QString text);

    Result(const Result& other) = default;
    Result& operator=(const Result& other) = default;

    Result(Result&& other) noexcept;
    Result& operator=(Result&& other) noexcept;

    ~Result() = default;

    /**
     * @brief Deserializes result from bytes.
     */
    static Result deserialize(QByteArray bytes);

    /**
     * @brief Serializes result to bytes.
     */
    QByteArray serialize() const;

    /// Result type.
    ResultType type() const { return m_type; }

    /// Result message.
    const QString& text() const { return m_text; }
};

}