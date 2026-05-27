#pragma once

#include <QByteArray>
#include <QUuid>

#include <utility>

namespace shared::crypto {

using KeyPair = std::pair<QByteArray, QByteArray>;

QByteArray encryptHybrid(const QByteArray& bytes, const QByteArray& publicKey);
QByteArray decryptHybrid(const QByteArray& bytes, const QByteArray& privateKey);
KeyPair deriveKeyPair(const QUuid& sessionId);

} // namespace shared::crypto
