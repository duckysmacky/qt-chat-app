#include "Result.h"

namespace shared {

/**
 * @brief Size of serialized result type field.
 */
static constexpr auto RESULT_TYPE_SIZE = 1;

/**
 * @brief Constructs a result object.
 */
Result::Result(const ResultType type, QString text)
    : m_type(type),
      m_text(std::move(text))
{
}

/**
 * @brief Move constructor.
 */
Result::Result(Result&& other) noexcept
    : m_type(other.m_type),
      m_text(std::move(other.m_text))
{}

/**
 * @brief Move assignment operator.
 */
Result& Result::operator=(Result&& other) noexcept
{
    if (this == &other) return *this;
    m_type = other.m_type;
    m_text = std::move(other.m_text);
    return *this;
}

/**
 * @brief Deserializes Result from bytes.
 */
Result Result::deserialize(QByteArray bytes)
{
    if (bytes.isEmpty()) {
        return Result{ResultType::ERROR, ""};
    }

    const auto type = static_cast<ResultType>(bytes[0]);
    bytes.remove(0, RESULT_TYPE_SIZE);

    return Result{type, QString::fromUtf8(bytes)};
}

/**
 * @brief Serializes Result into bytes.
 */
QByteArray Result::serialize() const
{
    QByteArray bytes;
    bytes.append(static_cast<char>(m_type));
    bytes.append(m_text.toUtf8());
    return bytes;
}

}