#pragma once

#include <QByteArray>
#include <QList>

#include "Packet.h"

namespace shared::util {

constexpr auto DELIMITER = '\xF0';

/**
 * @brief Parses raw byte data into a list of Message objects.
 */
QList<Packet> parse(const QByteArray& bytes);

/**
 * Encapsulates a packet into a proper payload. Serializes it
 * into bytes, then adds a delimiter
 * @param packet packet to be encapsulated
 * @return Encapsulated payload bytes
 */
QByteArray encapsulate(const Packet& packet);

}
