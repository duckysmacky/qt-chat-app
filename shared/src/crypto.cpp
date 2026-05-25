#include "crypto.h"

#include <QRandomGenerator>
#include <qaesencryption.h>
#include <qrsaencryption.h>

#include "KeyStore.h"

namespace shared::crypto {

namespace {

constexpr auto AES_KEY_SIZE = 32;
constexpr auto AES_IV_SIZE = 16;

}

QByteArray generateRandomBytes(const qsizetype size)
{
    QByteArray bytes;
    bytes.resize(size);

    for (qsizetype i = 0; i < size; ++i)
        bytes[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));

    return bytes;
}

KeyPair deriveKeyPair(const QUuid& sessionId)
{
    QByteArray publicKey;
    QByteArray privateKey;
    QRSAEncryption rsa(KeyStore::instance().keySize());
    rsa.generatePairKey(publicKey, privateKey, sessionId.toRfc4122());
    return {std::move(publicKey), std::move(privateKey)};
}

QByteArray encryptWithPublicKey(const QByteArray& bytes, const QByteArray& encryptionKey)
{
    if (bytes.isEmpty())
        return bytes;

    QRSAEncryption rsa(KeyStore::instance().keySize());
    return rsa.encode(bytes, encryptionKey);
}

QByteArray decryptWithPrivateKey(const QByteArray& bytes, const QByteArray& decryptionKey)
{
    if (bytes.isEmpty())
        return bytes;

    QRSAEncryption rsa(KeyStore::instance().keySize());
    return rsa.decode(bytes, decryptionKey);
}

QByteArray generateMasterKey()
{
    return generateRandomBytes(AES_KEY_SIZE);
}

qsizetype masterKeySize()
{
    return AES_KEY_SIZE;
}

QByteArray encryptWithMasterKey(const QByteArray& bytes, const QByteArray& masterKey)
{
    if (bytes.isEmpty())
        return bytes;

    if (masterKey.size() != AES_KEY_SIZE)
        return {};

    const QByteArray iv = generateRandomBytes(AES_IV_SIZE);
    QByteArray encrypted = QAESEncryption::Crypt(
        QAESEncryption::AES_256,
        QAESEncryption::CBC,
        bytes,
        masterKey,
        iv
    );

    return iv + encrypted;
}

QByteArray decryptWithMasterKey(const QByteArray& bytes, const QByteArray& masterKey)
{
    if (bytes.isEmpty())
        return bytes;

    if (masterKey.size() != AES_KEY_SIZE || bytes.size() <= AES_IV_SIZE)
        return {};

    const QByteArray iv = bytes.first(AES_IV_SIZE);
    const QByteArray encrypted = bytes.sliced(AES_IV_SIZE);
    const QByteArray decrypted = QAESEncryption::Decrypt(
        QAESEncryption::AES_256,
        QAESEncryption::CBC,
        encrypted,
        masterKey,
        iv
    );

    return QAESEncryption::RemovePadding(decrypted);
}

} // namespace shared::crypto
