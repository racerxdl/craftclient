#include "protocol/packet/encoder.h"
#include "common/objectpool.h"

using namespace ProtoRock;
using namespace ProtoRock::Common;
using namespace ProtoRock::Protocol;
using namespace ProtoRock::Protocol::Packet;

void writeVaruint32(std::shared_ptr<ByteBuffer> b, uint32_t v) {
    b->reserve(b->size() + 5); // It can take up to 5 bytes
    while (v >= 0x80) {
        b->push_back((uint8_t)((v & 0x7F) | 0x80));
        v >>= 7;
    }
    b->push_back((uint8_t)(v & 0x7F));
}

void Encoder::EnableEncryption(const Common::ByteBuffer key) {
    encrypt = std::make_shared<Crypto::Minecrypt>(key);
}

void Encoder::Encode(std::vector<Common::ByteBuffer> &packets) {
    auto b = ObjectPools::ByteBufferPool.Get();

    for (auto &packet: packets) {
        // Prefixed with varuint32 with packet length
        writeVaruint32(b, packet.size());
        b->insert(b->end(), packet.begin(), packet.end());
    }

    auto compressor = ObjectPools::CompressorPool.Get();
    auto compressed = compressor->Compress(b);
    ObjectPools::ByteBufferPool.Return(b);
    ObjectPools::CompressorPool.Return(compressor);

    if (encrypt) {
        encrypt->Encrypt(*compressed);
    }

    eo->WriteEncoded(*compressed);
    ObjectPools::ByteBufferPool.Return(compressed);
}