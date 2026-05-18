#pragma once

#include <QByteArray>
#include <QHash>
#include <QUuid>
#include <qrsaencryption.h>

#include <optional>

namespace shared {

/**
 * @class KeyStore
 * @brief Stores the local RSA key pair and peer public keys keyed by session id.
 *
 * One KeyStore instance belongs to one local session. It exposes the local key pair
 * and stores known peer public keys in a QHash<QUuid, QByteArray>, where QUuid is
 * the peer session id.
 */
class KeyStore
{
public:
    using KeySize = QRSAEncryption::Rsa;

private:
    KeySize m_keySize;
    QByteArray m_publicKey;
    QByteArray m_privateKey;
    QHash<QUuid, QByteArray> m_peerPublicKeys;

public:
    /// @brief Returns the process-wide key store instance.
    static KeyStore& instance();

    KeyStore(const KeyStore& other) = delete;
    KeyStore& operator=(const KeyStore& other) = delete;
    KeyStore(KeyStore&& other) = delete;
    KeyStore& operator=(KeyStore&& other) = delete;

    /// @brief Returns the configured RSA key size.
    KeySize keySize() const { return m_keySize; }

    /// @brief Returns the generated local public key.
    const QByteArray& publicKey() const { return m_publicKey; }

    /// @brief Returns the generated local private key.
    const QByteArray& privateKey() const { return m_privateKey; }

    /**
     * @brief Returns the public key for a peer session, if it is known.
     * @param peerSessionId Session UUID of the peer.
     */
    std::optional<QByteArray> peerPublicKey(const QUuid& peerSessionId) const;

    /// @brief Checks whether the local key pair was generated successfully.
    bool hasLocalKeyPair() const;

    /**
     * @brief Checks whether the public key for a peer session is already known.
     * @param peerSessionId Session UUID of the peer.
     */
    bool hasPeerPublicKey(const QUuid& peerSessionId) const;

    /**
     * @brief Stores or replaces the public key for a peer session.
     * @param peerSessionId Session UUID of the peer.
     * @param publicKey Public key bytes announced by that peer.
     */
    void setPeerPublicKey(const QUuid& peerSessionId, QByteArray publicKey);

    /**
     * @brief Removes the stored public key for a peer session.
     * @param peerSessionId Session UUID of the peer.
     */
    void removePeerPublicKey(const QUuid& peerSessionId);

    /// @brief Removes all stored peer public keys.
    void clearPeerPublicKeys();

    /// @brief Regenerates the local RSA key pair for this session.
    void regenerate();

private:

    /**
     * @brief Constructs a KeyStore and generates a fresh local key pair.
     * @param keySize RSA key size, RSA_2048 by default.
     */
    KeyStore(KeySize keySize = QRSAEncryption::Rsa::RSA_2048);

    void generateKeyPair();
};

}
