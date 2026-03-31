#pragma once

#include <QByteArray>
#include <QList>

#include "Message.h"

namespace shared::util {
/**
 * @brief Parses raw byte data into a list of Message objects.
 */
QList<Message> parse(const QByteArray& bytes);

}
