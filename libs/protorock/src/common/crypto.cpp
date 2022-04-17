#include "common/crypto.h"

#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/pem.h>

#include <mutex>

#include "common/common.h"

using namespace ProtoRock::Crypto;
using namespace ProtoRock::Common;

#define KEY_ALGO NID_X9_62_prime256v1

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

KeyPair ProtoRock::Crypto::generateES256KeyPair() {
    std::lock_guard<std::mutex> lock(sslMtx);

    EC_KEY *ec_key = EC_KEY_new_by_curve_name(KEY_ALGO);

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

KeyPair::KeyPair(Common::ByteBuffer publicKey, Common::ByteBuffer privateKey) {
    this->privateKey = privateKey;
    this->publicKey = publicKey;

    if (privateKey.empty()) {
        ecKey = LoadPublicKey(publicKey);
    } else {
        ecKey = LoadPrivateKey(privateKey);
    }
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
    auto rr = ByteBuffer(signature.begin(), signature.begin()+32);
    auto rs = ByteBuffer(signature.begin()+32, signature.end());
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
    auto c = X509_new();
    auto k = EVP_PKEY_new();

    EVP_PKEY_set1_EC_KEY(k, ecKey);
    X509_set_pubkey(c, k);

    int keyLen = i2d_X509(c, nullptr);
    ByteBuffer b;
    b.resize(keyLen);
    unsigned char *p = (unsigned char *)b.data();
    i2d_X509(c, &p);

    EVP_PKEY_free(k);
    X509_free(c);

    return b;
}