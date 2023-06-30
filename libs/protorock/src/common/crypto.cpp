#include "common/crypto.h"

#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/pem.h>
#include <openssl/crypto.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/aes.h>

#include <mutex>

#include "common/common.h"

using namespace ProtoRock::Crypto;
using namespace ProtoRock::Common;

std::mutex sslMtx;  // OpenSSL is not thread-safe

EC_KEY *LoadPrivateKey(const ByteBuffer &b) {
    EC_KEY *ec_key;
    BIO *privateBIO = BIO_new(BIO_s_mem());
    BIO_write(privateBIO, b.data(), b.size());
    ec_key = PEM_read_bio_ECPrivateKey(privateBIO, nullptr, nullptr, nullptr);
    BIO_free(privateBIO);
    return ec_key;
}

EC_KEY *LoadPublicKey(const ByteBuffer &b) {
    EC_KEY *ec_key;
    BIO *privateBIO = BIO_new(BIO_s_mem());
    BIO_write(privateBIO, b.data(), b.size());
    ec_key = PEM_read_bio_EC_PUBKEY(privateBIO, nullptr, nullptr, nullptr);
    BIO_free(privateBIO);
    if (ec_key == nullptr) {  // try d2i
        auto buff = b.data();
        ec_key = d2i_EC_PUBKEY(nullptr, &buff, b.size());
    }
    return ec_key;
}

ByteBuffer privateKeyToBytes(EC_KEY *ec_key) {
    BIO *privateBIO = BIO_new(BIO_s_mem());
    PEM_write_bio_ECPrivateKey(privateBIO, ec_key, nullptr, nullptr, 0, nullptr, nullptr);
    int keyLen = BIO_pending(privateBIO);
    ByteBuffer b;
    b.resize(keyLen);
    BIO_read(privateBIO, b.data(), keyLen);
    BIO_free(privateBIO);

    return b;
}

ByteBuffer publicKeyToBytes(EC_KEY *ec_key) {
    BIO *privateBIO = BIO_new(BIO_s_mem());
    PEM_write_bio_EC_PUBKEY(privateBIO, ec_key);
    int keyLen = BIO_pending(privateBIO);
    ByteBuffer b;
    b.resize(keyLen);
    BIO_read(privateBIO, b.data(), keyLen);
    BIO_free(privateBIO);

    return b;
}

ByteBuffer sha256(const ByteBuffer &data) {
    ByteBuffer result;
    result.resize(SHA256_DIGEST_LENGTH);
    SHA256_CTX hasher;
    SHA256_Init(&hasher);
    SHA256_Update(&hasher, data.data(), data.size());
    SHA256_Final(result.data(), &hasher);
    return result;
}
KeyPair ProtoRock::Crypto::generateP256KeyPair() {
    std::lock_guard<std::mutex> lock(sslMtx);

    EC_KEY *ec_key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);

    if (!EC_KEY_generate_key(ec_key)) {
        EC_KEY_free(ec_key);
        throw Exception("cannot generate key");
    }
    auto pair = KeyPair(
        publicKeyToBytes(ec_key),
        privateKeyToBytes(ec_key));
    EC_KEY_free(ec_key);
    return pair;
}
KeyPair ProtoRock::Crypto::generateP384KeyPair() {
    std::lock_guard<std::mutex> lock(sslMtx);

    EC_KEY *ec_key = EC_KEY_new_by_curve_name(NID_secp384r1);

    if (!EC_KEY_generate_key(ec_key)) {
        EC_KEY_free(ec_key);
        throw Exception("cannot generate key");
    }
    auto pair = KeyPair(
        publicKeyToBytes(ec_key),
        privateKeyToBytes(ec_key));
    EC_KEY_free(ec_key);
    return pair;
}

void KeyPair::fill() {
    auto point = EC_KEY_get0_public_key(ecKey);
    auto group = EC_KEY_get0_group(ecKey);
    auto bnx = BN_new();
    auto bny = BN_new();
    EC_POINT_get_affine_coordinates(group, point, bnx, bny, nullptr);

    x.resize(BN_num_bytes(bnx));
    y.resize(BN_num_bytes(bny));

    BN_bn2bin(bnx, x.data());
    BN_bn2bin(bny, y.data());

    BN_free(bnx);
    BN_free(bny);
}
void KeyPair::loadFromBuffers(Common::ByteBuffer publicKey, Common::ByteBuffer privateKey) {
    this->privateKey = privateKey;
    this->publicKey = publicKey;

    if (privateKey.empty()) {
        ecKey = LoadPublicKey(publicKey);
    } else {
        ecKey = LoadPrivateKey(privateKey);
    }
    if (!ecKey) {
        throw Exception("cannot read key from data");
    }
    fill();
}

KeyPair::KeyPair(const KeyPair &k) {
    loadFromBuffers(k.publicKey, k.privateKey);
}

KeyPair::KeyPair(const std::string &x509publicKey) {
    BIO *privateBIO = BIO_new(BIO_s_mem());
    BIO_write(privateBIO, x509publicKey.data(), x509publicKey.size());

    ecKey = PEM_read_bio_EC_PUBKEY(privateBIO, nullptr, nullptr, nullptr);
    if (!ecKey) {
        throw Exception("cannot read public key from x509 data");
    }
    BIO_free(privateBIO);
    fill();
}

KeyPair::KeyPair(Common::ByteBuffer publicKey, Common::ByteBuffer privateKey) {
    loadFromBuffers(publicKey, privateKey);
}

KeyPair::~KeyPair() {
    if (ecKey != nullptr) {
        EC_KEY_free(ecKey);
    }
}

ByteBuffer KeyPair::Sign(const ByteBuffer &data) const {
    if (ecKey == nullptr || EC_KEY_get0_private_key(ecKey) == nullptr) {
        throw Exception("cannot sign: private key not loaded");
    }
    auto hash = sha256(data);

    auto signature = ECDSA_do_sign(hash.data(), hash.size(), ecKey);
    auto r = ECDSA_SIG_get0_r(signature);
    auto s = ECDSA_SIG_get0_s(signature);
    auto rr = ByteBuffer(BN_num_bytes(r), 0);
    auto rs = ByteBuffer(BN_num_bytes(s), 0);
    BN_bn2bin(r, rr.data());
    BN_bn2bin(s, rs.data());

    rr.insert(rr.end(), rs.begin(), rs.end());

    ECDSA_SIG_free(signature);
    return rr;
}

bool KeyPair::Verify(const ByteBuffer &data, const ByteBuffer &signature) const {
    if (ecKey == nullptr) {
        throw Exception("cannot verify: public key not loaded");
    }
    auto rr = ByteBuffer(signature.begin(), signature.begin() + 32);
    auto rs = ByteBuffer(signature.begin() + 32, signature.end());
    auto r = BN_bin2bn(rr.data(), rr.size(), nullptr);
    auto s = BN_bin2bn(rs.data(), rs.size(), nullptr);
    auto hash = sha256(data);

    auto sig = ECDSA_SIG_new();
    ECDSA_SIG_set0(sig, r, s);

    auto result = ECDSA_do_verify(hash.data(), hash.size(), sig, ecKey) == 1;

    ECDSA_SIG_free(sig);
    BN_free(r);
    BN_free(s);

    return result;
}

ByteBuffer KeyPair::PKIXPublicKey() const {
    ByteBuffer b;
    b.resize(i2d_EC_PUBKEY(ecKey, nullptr));
    auto buff = b.data();
    i2d_EC_PUBKEY(ecKey, &buff);

    return b;
}

std::string KeyPair::PublicToPEM() const {
    BIO *bio = BIO_new(BIO_s_mem());
    PEM_write_bio_EC_PUBKEY(bio, ecKey);

    int keyLen = BIO_pending(bio);
    ByteBuffer b;
    b.resize(keyLen);
    BIO_read(bio, b.data(), keyLen);
    BIO_free(bio);

    return std::string(b.begin(), b.end());
}

std::string KeyPair::PrivateToPEM() const {
    BIO *bio = BIO_new(BIO_s_mem());
    PEM_write_bio_ECPrivateKey(bio, ecKey, nullptr, nullptr, 0, nullptr, nullptr);

    int keyLen = BIO_pending(bio);
    ByteBuffer b;
    b.resize(keyLen);
    BIO_read(bio, b.data(), keyLen);
    BIO_free(bio);

    return std::string(b.begin(), b.end());
}

ByteBuffer KeyPair::DHComputeKey(const KeyPair &peer) {
    ByteBuffer b;
    auto field_size = EC_GROUP_get_degree(EC_KEY_get0_group(ecKey));
	b.resize((field_size+7)/8);

    auto generatedLength = ECDH_compute_key(b.data(), b.size(), EC_KEY_get0_public_key(peer.ecKey), ecKey, nullptr);

    if (generatedLength <= 0) {
        throw Exception("could not generate DH Key");
    }

    return b;
}

Minecrypt::Minecrypt(const Common::ByteBuffer &keyBytes) {
    // Ok so this is funny. I spent a lot of time trying to figure out what's minecraft doing.
    // And basically it does GCM but without verification tag, which is basically CTR.
    // Although its CTR, GCM actually initializes the last 4 bytes with a number 2 in bigendian
    // So that's what I do here

    this->keyBytes = keyBytes;
    sendCounter = 0;
    counterBuff.resize(8);
    hashBuff.resize(32);
    ivBytes.resize(16);
    memset(ivBytes.data(), 0, 16);
    memcpy(ivBytes.data(), keyBytes.data(), 16);
    ivBytes[12] = 0;
    ivBytes[13] = 0;
    ivBytes[14] = 0;
    ivBytes[15] = 2;
    ctx = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(ctx);
    EVP_EncryptInit(ctx, EVP_aes_256_ctr(), keyBytes.data(), ivBytes.data());
    EVP_CIPHER_CTX_set_padding(ctx, 0);
}

void Minecrypt::Encrypt(Common::ByteBuffer &buffer) {
    // Counter as Little Endian
    counterBuff[0] = (uint8_t)(sendCounter >> 0);
    counterBuff[1] = (uint8_t)(sendCounter >> 8);
    counterBuff[2] = (uint8_t)(sendCounter >> 16);
    counterBuff[3] = (uint8_t)(sendCounter >> 24);
    counterBuff[4] = (uint8_t)(sendCounter >> 32);
    counterBuff[5] = (uint8_t)(sendCounter >> 40);
    counterBuff[6] = (uint8_t)(sendCounter >> 48);
    counterBuff[7] = (uint8_t)(sendCounter >> 56);
    sendCounter++;

    SHA256_CTX hasher;
    SHA256_Init(&hasher);
    SHA256_Update(&hasher, counterBuff.data(), 8);
    SHA256_Update(&hasher, buffer.data(), buffer.size());
    SHA256_Update(&hasher, keyBytes.data(), keyBytes.size());
    SHA256_Final(hashBuff.data(), &hasher);
    buffer.insert(buffer.end(), hashBuff.begin(), hashBuff.begin()+8);
    Minecrypt::XORBuffer(buffer);
}

void Minecrypt::XORBuffer(Common::ByteBuffer &data) {
    int l = data.size();
    EVP_EncryptUpdate(ctx, data.data(), &l, data.data(), data.size());
}

Minecrypt::~Minecrypt() {
    EVP_CIPHER_CTX_free(ctx);
}

void Minecrypt::Decrypt(Common::ByteBuffer &buffer) {
    Minecrypt::XORBuffer(buffer);
}

void Minecrypt::Verify(const Common::ByteBuffer &buffer) {
    if (buffer.size() < 8) {
        throw Exception("expected buffer to be at least 8 bytes long");
    }

    counterBuff[0] = (uint8_t)(sendCounter >> 0);
    counterBuff[1] = (uint8_t)(sendCounter >> 8);
    counterBuff[2] = (uint8_t)(sendCounter >> 16);
    counterBuff[3] = (uint8_t)(sendCounter >> 24);
    counterBuff[4] = (uint8_t)(sendCounter >> 32);
    counterBuff[5] = (uint8_t)(sendCounter >> 40);
    counterBuff[6] = (uint8_t)(sendCounter >> 48);
    counterBuff[7] = (uint8_t)(sendCounter >> 56);
    sendCounter++;

    SHA256_CTX hasher;
    SHA256_Init(&hasher);
    SHA256_Update(&hasher, counterBuff.data(), 8);
    SHA256_Update(&hasher, buffer.data(), buffer.size() - 8);  // minus hash
    SHA256_Update(&hasher, keyBytes.data(), keyBytes.size());
    SHA256_Final(hashBuff.data(), &hasher);

    // Compare hashes
    if (memcmp(buffer.data() + buffer.size() - 8, hashBuff.data(), 8) != 0) {
        throw Exception(fmt::format("checksum does not match."));
    };
}