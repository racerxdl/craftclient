#pragma once

#include <cstdint>

#include "common/common.h"
#include "common/crypto.h"

namespace ProtoRock {
namespace Protocol {
namespace Packet {

struct EncoderOutput {
    virtual void WriteEncoded(const Common::ByteBuffer &b) = 0;
};

class Encoder {
    private:
        std::shared_ptr<EncoderOutput> eo;
        std::shared_ptr<Crypto::Minecrypt> encrypt;
    public:
        Encoder(std::shared_ptr<EncoderOutput> eo) : eo(eo) {}
        void Encode(std::vector<Common::ByteBuffer> &packets);
        void EnableEncryption(const Common::ByteBuffer key);
};

}  // namespace Packet
}  // namespace Protocol
}  // namespace ProtoRock