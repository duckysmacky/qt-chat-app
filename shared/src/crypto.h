#pragma once

#include <QByteArray>
#include <QUuid>

#include <utility>

namespace shared::crypto {

using KeyPair = std::pair<QByteArray, QByteArray>;

QByteArray encryptBytes(const QByteArray& bytes, const QByteArray& encryptionKey);
QByteArray decryptBytes(const QByteArray& bytes, const QByteArray& decryptionKey);
KeyPair deriveKeyPair(const QUuid& sessionId);

} // namespace shared::crypto
