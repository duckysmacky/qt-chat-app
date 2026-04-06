#pragma once

#include <QByteArray>
#include <QList>

#include "Packet.h"

namespace shared::util {

constexpr auto DELIMITER = '\xFF';

/**
 * @brief Parses raw byte data into a list of Message objects.
 */
QList<Packet> parse(const QByteArray& bytes);

}
