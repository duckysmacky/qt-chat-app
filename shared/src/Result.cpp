#include "Result.h"

namespace shared {

static constexpr auto RESULT_TYPE_SIZE = 1;

Result::Result(const ResultType type, QString text)
    : m_type(type),
    m_text(std::move(text))
{
}

Result::Result(Result&& other) noexcept
    : m_type(other.m_type),
      m_text(std::move(other.m_text))
{}

Result& Result::operator=(Result&& other) noexcept
{
    if (this == &other) return *this;
    m_type = other.m_type;
    m_text = std::move(other.m_text);
    return *this;
}

Result Result::deserialize(QByteArray bytes)
{
    if (bytes.isEmpty()) {
        return Result{ResultType::ERROR, ""};
    }

    const auto type = static_cast<ResultType>(bytes[0]);
    bytes.remove(0, RESULT_TYPE_SIZE);

    return Result{type, QString::fromUtf8(bytes)};
}

QByteArray Result::serialize() const
{
    QByteArray bytes;
    bytes.append(static_cast<char>(m_type));
    bytes.append(m_text.toUtf8());
    return bytes;
}

}
