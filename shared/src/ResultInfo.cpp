#include "ResultInfo.h"

namespace shared {

static constexpr auto RESULT_TYPE_SIZE = 1;

ResultInfo::ResultInfo(ResultType type, QString text)
    : m_type(type),
    m_text(std::move(text))
{
}

ResultInfo ResultInfo::deserialize(QByteArray bytes)
{
    if (bytes.isEmpty()) {
        return ResultInfo(ResultType::ERROR, "");
    }

    const auto type = static_cast<ResultType>(bytes[0]);
    bytes.remove(0, RESULT_TYPE_SIZE);

    return ResultInfo(type, QString::fromUtf8(bytes));
}

QByteArray ResultInfo::serialize() const
{
    QByteArray bytes;
    bytes.append(static_cast<char>(m_type));
    bytes.append(m_text.toUtf8());
    return bytes;
}

}
