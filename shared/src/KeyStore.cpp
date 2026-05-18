#include "KeyStore.h"

#include <utility>

namespace shared {

KeyStore& KeyStore::instance()
{
    static KeyStore instance;
    return instance;
}

KeyStore::KeyStore(const KeySize keySize)
    : m_keySize(keySize)
{
    generateKeyPair();
}

bool KeyStore::hasLocalKeyPair() const
{
    return !m_publicKey.isEmpty() && !m_privateKey.isEmpty();
}

std::optional<QByteArray> KeyStore::peerPublicKey(const QUuid& peerSessionId) const
{
    const auto it = m_peerPublicKeys.constFind(peerSessionId);
    if (it == m_peerPublicKeys.constEnd() || it.value().isEmpty())
        return std::nullopt;

    return it.value();
}

bool KeyStore::hasPeerPublicKey(const QUuid& peerSessionId) const
{
    return peerPublicKey(peerSessionId).has_value();
}

void KeyStore::setPeerPublicKey(const QUuid& peerSessionId, QByteArray publicKey)
{
    if (publicKey.isEmpty())
        return;

    m_peerPublicKeys.insert(peerSessionId, std::move(publicKey));
}

void KeyStore::removePeerPublicKey(const QUuid& peerSessionId)
{
    m_peerPublicKeys.remove(peerSessionId);
}

void KeyStore::clearPeerPublicKeys()
{
    m_peerPublicKeys.clear();
}

void KeyStore::regenerate()
{
    m_publicKey.clear();
    m_privateKey.clear();
    generateKeyPair();
}

void KeyStore::generateKeyPair()
{
    QRSAEncryption rsa(m_keySize);
    rsa.generatePairKey(m_publicKey, m_privateKey);
}

}
