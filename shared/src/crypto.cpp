#include "crypto.h"

#include <qrsaencryption.h>

#include "KeyStore.h"

namespace shared::crypto {

QByteArray encryptBytes(const QByteArray& bytes, const QByteArray& encryptionKey)
{
    if (bytes.isEmpty())
        return bytes;

    QRSAEncryption rsa(KeyStore::instance().keySize());
    return rsa.encode(bytes, encryptionKey);
}

QByteArray decryptBytes(const QByteArray& bytes, const QByteArray& decryptionKey)
{
    if (bytes.isEmpty())
        return bytes;

    QRSAEncryption rsa(KeyStore::instance().keySize());
    return rsa.decode(bytes, decryptionKey);
}

KeyPair deriveKeyPair(const QUuid& sessionId)
{
    QByteArray publicKey;
    QByteArray privateKey;
    QRSAEncryption rsa(KeyStore::instance().keySize());
    rsa.generatePairKey(publicKey, privateKey, sessionId.toRfc4122());
    return {std::move(publicKey), std::move(privateKey)};
}

} // namespace shared::crypto
