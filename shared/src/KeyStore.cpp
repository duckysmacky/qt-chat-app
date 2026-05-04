#include "KeyStore.h"

#include <utility>

namespace shared {

KeyStore::KeyStore(const KeySize keySize)
    : m_keySize(keySize)
{
    generateKeyPair();
}

bool KeyStore::hasLocalKeyPair() const
{
    return !m_publicKey.isEmpty() && !m_privateKey.isEmpty();
}

bool KeyStore::hasPeerPublicKey() const
{
    return m_peerPublicKey.has_value() && !m_peerPublicKey->isEmpty();
}

void KeyStore::setPeerPublicKey(QByteArray publicKey)
{
    m_peerPublicKey = std::move(publicKey);
}

void KeyStore::clearPeerPublicKey()
{
    m_peerPublicKey.reset();
}

void KeyStore::regenerate()
{
    m_publicKey.clear();
    m_privateKey.clear();
    generateKeyPair();
}

std::optional<QByteArray> KeyStore::encryptForPeer(const QByteArray& plainBytes) const
{
    if (!hasPeerPublicKey())
        return std::nullopt;

    QRSAEncryption rsa = makeRsa();
    return rsa.encode(plainBytes, m_peerPublicKey.value());
}

std::optional<QByteArray> KeyStore::decryptForSelf(const QByteArray& encryptedBytes) const
{
    if (!hasLocalKeyPair())
        return std::nullopt;

    QRSAEncryption rsa = makeRsa();
    return rsa.decode(encryptedBytes, m_privateKey);
}

QRSAEncryption KeyStore::makeRsa() const
{
    return QRSAEncryption(m_keySize);
}

void KeyStore::generateKeyPair()
{
    QRSAEncryption rsa = makeRsa();
    rsa.generatePairKey(m_publicKey, m_privateKey);
}

}
