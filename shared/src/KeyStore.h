#pragma once

#include <QByteArray>

#include <optional>

#include <qrsaencryption.h>

namespace shared {

/**
 * @class KeyStore
 * @brief Stores the local RSA key pair and the peer public key for one session-to-session channel.
 *
 * One KeyStore instance is intended to be stored in QHash<QUuid, shared::KeyStore>,
 * where QUuid is the session id of the other side.
 */
class KeyStore
{
public:
    using KeySize = QRSAEncryption::Rsa;

    /**
     * @brief Constructs a KeyStore and generates a fresh local key pair.
     * @param keySize RSA key size, RSA_2048 by default.
     */
    explicit KeyStore(KeySize keySize = QRSAEncryption::Rsa::RSA_2048);

    /// @brief Returns the configured RSA key size.
    KeySize keySize() const { return m_keySize; }

    /// @brief Returns the generated local public key.
    const QByteArray& publicKey() const { return m_publicKey; }

    /// @brief Returns the generated local private key.
    const QByteArray& privateKey() const { return m_privateKey; }

    /// @brief Returns the stored peer public key, if it is known.
    const std::optional<QByteArray>& peerPublicKey() const { return m_peerPublicKey; }

    /// @brief Checks whether the local key pair was generated successfully.
    bool hasLocalKeyPair() const;

    /// @brief Checks whether the peer public key is already known.
    bool hasPeerPublicKey() const;

    /// @brief Stores or replaces the peer public key.
    void setPeerPublicKey(QByteArray publicKey);

    /// @brief Removes the stored peer public key.
    void clearPeerPublicKey();

    /// @brief Regenerates the local RSA key pair for this session.
    void regenerate();

    /**
     * @brief Encrypts bytes for the peer using the peer public key.
     * @param plainBytes Raw bytes to encrypt.
     * @return Encrypted bytes or std::nullopt if the peer key is not known.
     */
    std::optional<QByteArray> encryptForPeer(const QByteArray& plainBytes) const;

    /**
     * @brief Decrypts bytes addressed to this side using the local private key.
     * @param encryptedBytes Raw encrypted bytes.
     * @return Decrypted bytes or std::nullopt if the local key pair is missing.
     */
    std::optional<QByteArray> decryptForSelf(const QByteArray& encryptedBytes) const;

private:
    QRSAEncryption makeRsa() const;
    void generateKeyPair();

private:
    KeySize m_keySize;
    QByteArray m_publicKey;
    QByteArray m_privateKey;
    std::optional<QByteArray> m_peerPublicKey;
};

}
