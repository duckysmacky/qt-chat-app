#include "Hasher.h"

#include <QCryptographicHash>

QString Hasher::sha256(QString text)
{
    const QByteArray hash = QCryptographicHash::hash(text.toUtf8(), QCryptographicHash::Sha256);
    return QString::fromLatin1(hash.toHex());
}
