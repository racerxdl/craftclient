#pragma once

#include <cstdint>

#include "common/common.h"
#include "common/crypto.h"

namespace ProtoRock {
namespace Protocol {
namespace Packet {

class Decoder {
    private:
        std::shared_ptr<Crypto::Minecrypt> encrypt;
    public:
        Decoder() {}
        std::vector<Common::SharedByteBuffer> Decode(Common::SharedByteBuffer b);
        void EnableEncryption(const Common::ByteBuffer key);
};

}  // namespace Packet
}  // namespace Protocol
}  // namespace ProtoRock