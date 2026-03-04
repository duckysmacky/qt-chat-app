#pragma once

#include <QList>
#include <QByteArray>

#include "Message.h"

namespace middleware {
QList<shared::Message> parse(const QByteArray& bytes);
}
