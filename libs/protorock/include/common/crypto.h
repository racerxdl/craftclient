#pragma once

#include <openssl/ec.h>

#include "common/base64.h"
#include "common/common.h"

namespace ProtoRock {
namespace Crypto {
struct KeyPair {
   private:
    EC_KEY *ecKey = nullptr;
    Common::ByteBuffer x;
    Common::ByteBuffer y;
    void loadFromBuffers(Common::ByteBuffer publicKey, Common::ByteBuffer privateKey);
    void fill();
   public:
    Common::ByteBuffer publicKey;
    Common::ByteBuffer privateKey;

    KeyPair() {}
    KeyPair(const std::string &x509publicKey);
    KeyPair(Common::ByteBuffer publicKey, Common::ByteBuffer privateKey);
    KeyPair(Common::ByteBuffer publicKey) : KeyPair(publicKey, Common::ByteBuffer()) {}
    ~KeyPair();

    std::string X() const { return Base64::Encode(x); }
    std::string Y() const { return Base64::Encode(y); }
    Common::ByteBuffer PKIXPublicKey() const;
    Common::ByteBuffer Sign(const Common::ByteBuffer &data) const;
    Common::ByteBuffer Sign(const std::string &data) const { return Sign(Common::ByteBuffer(data.begin(), data.end())); };
    bool Verify(const Common::ByteBuffer &data, const Common::ByteBuffer &signature) const;
    std::string PublicToPEM() const;
    std::string PrivateToPEM() const;
};

KeyPair generateES384KeyPair();
}  // namespace Crypto
}  // namespace ProtoRock