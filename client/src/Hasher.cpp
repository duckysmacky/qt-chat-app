#include "Hasher.h"

#include <QCryptographicHash>

/**
 * @brief Computes SHA-256 hash of the given text.
 * @param text Input string to hash.
 * @return Hex-encoded SHA-256 hash string.
 */
QString Hasher::sha256(QString text)
{
    const QByteArray hash = QCryptographicHash::hash(text.toUtf8(), QCryptographicHash::Sha256);
    return QString::fromLatin1(hash.toHex());
}
