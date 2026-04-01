#pragma once

#include <QByteArray>
#include <QList>

#include "Packet.h"

constexpr auto DELIMITER = '\xFF';

namespace shared::util {
/**
 * @brief Parses raw byte data into a list of Message objects.
 */
QList<Packet> parse(const QByteArray& bytes);

}
