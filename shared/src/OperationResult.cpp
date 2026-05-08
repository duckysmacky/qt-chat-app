#include "OperationResult.h"

#include <utility>

namespace shared {

static constexpr auto OPERATION_RESULT_TYPE_SIZE = 1;

OperationResult::OperationResult(const OperationResultType type, QString text)
    : m_type(type),
    m_text(std::move(text))
{
}

OperationResult::OperationResult(OperationResult&& other) noexcept
    : m_type(other.m_type),
      m_text(std::move(other.m_text))
{}

OperationResult& OperationResult::operator=(OperationResult&& other) noexcept
{
    if (this == &other) return *this;
    m_type = other.m_type;
    m_text = std::move(other.m_text);
    return *this;
}

OperationResult OperationResult::deserialize(QByteArray bytes)
{
    if (bytes.isEmpty()) {
        return OperationResult{OperationResultType::ERROR, ""};
    }

    const auto type = static_cast<OperationResultType>(bytes[0]);
    bytes.remove(0, OPERATION_RESULT_TYPE_SIZE);

    return OperationResult{type, QString::fromUtf8(bytes)};
}

QByteArray OperationResult::serialize() const
{
    QByteArray bytes;
    bytes.append(static_cast<char>(m_type));
    bytes.append(m_text.toUtf8());
    return bytes;
}

}
