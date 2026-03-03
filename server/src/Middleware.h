#pragma once

#include <QByteArray>
#include <QStringList>

namespace middleware {
QList<QByteArray> parse(const QByteArray& bytes);
};
