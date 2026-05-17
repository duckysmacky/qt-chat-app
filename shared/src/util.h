/**
 * @file util.h
 * @brief Utility functions for packet parsing, encapsulation, and username validation.
 */

#pragma once

#include <QByteArray>
#include <QList>
#include <QString>

#include "Packet.h"

namespace shared::util {

/// Delimiter character marking packet boundaries in a byte stream.
constexpr auto DELIMITER = '\xF0';

/// Escape character used for byte stuffing.
constexpr auto ESCAPE = '\xF1';

/// Encoded representation of DELIMITER after escaping.
constexpr auto ESCAPED_DELIMITER = '\x00';

/// Encoded representation of ESCAPE after escaping.
constexpr auto ESCAPED_ESCAPE = '\x01';

/**
 * @brief Parses raw byte data into a list of Packet objects.
 * @param bytes The raw byte array to parse.
 * @return List of successfully parsed packets. Incomplete packets are ignored.
 */
QList<Packet> parse(const QByteArray& bytes);

/**
 * @brief Parses streaming byte data, consuming successfully processed bytes.
 * @param bytes Byte array passed by reference; processed bytes are removed.
 * @return List of parsed packets. Partial packets remain in the buffer.
 */
QList<Packet> parseStream(QByteArray& bytes);

/**
 * @brief Encapsulates a packet for transmission.
 * @param packet The packet to encapsulate.
 * @return Encapsulated payload bytes with delimiter and byte stuffing applied.
 */
QByteArray encapsulate(const Packet& packet);

/**
 * @brief Converts username to lowercase for case-insensitive handling.
 * @param username The username to normalize.
 * @return Normalized lowercase username.
 */
QString normalizeUsername(QString username);

/**
 * @brief Validates username format (length, allowed characters, etc.).
 * @param username The username to validate.
 * @return true if valid, false otherwise.
 */
bool isValidUsername(const QString& username);

} // namespace shared::util