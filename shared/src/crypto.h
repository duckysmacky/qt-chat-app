#pragma once

#include <QByteArray>
#include <QUuid>

#include <utility>

namespace shared::crypto {

using KeyPair = std::pair<QByteArray, QByteArray>;

QByteArray generateRandomBytes(const qsizetype size);

KeyPair deriveKeyPair(const QUuid& sessionId);

QByteArray encryptWithPublicKey(const QByteArray& bytes, const QByteArray& encryptionKey);
QByteArray decryptWithPrivateKey(const QByteArray& bytes, const QByteArray& decryptionKey);

QByteArray generateMasterKey();

QByteArray encryptWithMasterKey(const QByteArray& bytes, const QByteArray& masterKey);
QByteArray decryptWithMasterKey(const QByteArray& bytes, const QByteArray& masterKey);

} // namespace shared::crypto
