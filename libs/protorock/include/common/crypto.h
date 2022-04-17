#pragma once

#include "common/common.h"
#include "common/base64.h"

#include <openssl/ec.h>

namespace ProtoRock {
namespace Crypto {
    struct KeyPair {
    private:
        EC_KEY *ecKey = nullptr;
        Common::ByteBuffer x;
        Common::ByteBuffer y;
    public:
        Common::ByteBuffer publicKey;
        Common::ByteBuffer privateKey;

        KeyPair() {}
        KeyPair(Common::ByteBuffer publicKey, Common::ByteBuffer privateKey);
        ~KeyPair();

        std::string X() const { return Base64::Encode(x); }
        std::string Y() const { return Base64::Encode(y); }
        Common::ByteBuffer PKIXPublicKey() const;

        Common::ByteBuffer Sign(const Common::ByteBuffer &data) const ;
        Common::ByteBuffer Sign(const std::string &data) const { return Sign(Common::ByteBuffer(data.begin(), data.end())); };
        bool Verify(const Common::ByteBuffer &data, const Common::ByteBuffer &signature) const ;
    };

    KeyPair generateES256KeyPair();
}
}  // namespace ProtoRock