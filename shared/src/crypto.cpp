#include "crypto.h"

#include <QRandomGenerator>
#include <qaesencryption.h>
#include <qrsaencryption.h>

#include "KeyStore.h"

namespace shared::crypto {

namespace {

constexpr auto AES_KEY_SIZE = 32;
constexpr auto AES_IV_SIZE = 16;
constexpr auto RSA_2048_OUTPUT_SIZE = 256;

QByteArray generateRandomBytes(const qsizetype size)
{
    QByteArray bytes(size, Qt::Uninitialized);
    for (qsizetype i = 0; i < size; ++i)
        bytes[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    return bytes;
}

} // namespace

QByteArray encryptHybrid(const QByteArray& bytes, const QByteArray& publicKey)
{
    if (bytes.isEmpty() || publicKey.isEmpty())
        return {};

    const QByteArray aesKey = generateRandomBytes(AES_KEY_SIZE);
    const QByteArray iv     = generateRandomBytes(AES_IV_SIZE);

    const QByteArray aesEncrypted = QAESEncryption::Crypt(
        QAESEncryption::AES_256, QAESEncryption::CBC, bytes, aesKey, iv);

    QRSAEncryption rsa(KeyStore::instance().keySize());
    const QByteArray encryptedKey = rsa.encode(aesKey, publicKey, QRSAEncryption::BlockSize::OneByte);

    return encryptedKey + iv + aesEncrypted;
}

QByteArray decryptHybrid(const QByteArray& bytes, const QByteArray& privateKey)
{
    if (bytes.size() <= RSA_2048_OUTPUT_SIZE + AES_IV_SIZE || privateKey.isEmpty())
        return {};

    const QByteArray encryptedKey = bytes.first(RSA_2048_OUTPUT_SIZE);
    const QByteArray iv           = bytes.sliced(RSA_2048_OUTPUT_SIZE, AES_IV_SIZE);
    const QByteArray aesEncrypted = bytes.sliced(RSA_2048_OUTPUT_SIZE + AES_IV_SIZE);

    QRSAEncryption rsa(KeyStore::instance().keySize());
    const QByteArray aesKey = rsa.decode(encryptedKey, privateKey, QRSAEncryption::BlockSize::OneByte);

    const QByteArray decrypted = QAESEncryption::Decrypt(
        QAESEncryption::AES_256, QAESEncryption::CBC, aesEncrypted, aesKey, iv);

    return QAESEncryption::RemovePadding(decrypted);
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
