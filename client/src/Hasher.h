#pragma once

#include <QString>

/**
 * @class Hasher
 * @brief Provides static hashing utility functions.
 */
class Hasher
{
public:
    /**
     * @brief Computes the SHA-256 hash of the given text.
     * @param text The input string to hash.
     * @return The hexadecimal representation of the SHA-256 hash.
     */
    static QString sha256(QString text);
};