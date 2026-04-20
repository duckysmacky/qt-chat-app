#pragma once

#include <QString>

/**
 * @class Hasher
 * @brief Utility class for hashing operations.
 */
class Hasher
{
public:
    /**
     * @brief Computes SHA-256 hash of input text.
     * @param text Input string to hash.
     * @return Hex-encoded SHA-256 hash.
     */
    static QString sha256(QString text);
};
