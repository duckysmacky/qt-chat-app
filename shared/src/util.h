#pragma once

#include <QByteArray>
#include <QList>
#include <QString>

#include "Packet.h"

namespace shared::util {

constexpr auto DELIMITER = '\xF0';
constexpr auto ESCAPE = '\xF1';
constexpr auto ESCAPED_DELIMITER = '\x00';
constexpr auto ESCAPED_ESCAPE = '\x01';

/**
 * @brief Parses raw byte data into a list of Message objects.
 */
QList<Packet> parse(const QByteArray& bytes);
QList<Packet> parseStream(QByteArray& bytes);

/**
 * Encapsulates a packet into a proper payload. Serializes it
 * into bytes, then adds a delimiter
 * @param packet packet to be encapsulated
 * @return Encapsulated payload bytes
 */
QByteArray encapsulate(const Packet& packet);

QString normalizeUsername(QString username);
bool isValidUsername(const QString& username);

}
