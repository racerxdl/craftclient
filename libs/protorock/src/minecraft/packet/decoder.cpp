#include "protocol/packet/decoder.h"

#include "common/objectpool.h"

using namespace ProtoRock;
using namespace ProtoRock::Common;
using namespace ProtoRock::Protocol;
using namespace ProtoRock::Protocol::Packet;

uint32_t readVaruint32(Common::SharedByteBuffer buff) {
    uint32_t v = 0;
    for (int i = 0; i < 35; i += 7) {
        auto b = buff->at(0);
        buff->erase(buff->begin());
        v |= (b & 0x7F) << i;
        if ((b & 0x80) == 0) {
            break;
        }
    }
    return v;
}

std::vector<Common::SharedByteBuffer> Decoder::Decode(Common::SharedByteBuffer b) {
    std::vector<Common::SharedByteBuffer> result;

    if (b->size() == 0) {
        throw Exception("empty buffer");
    }

    if (b->operator[](0) != 0xFE) {  // header
        throw Exception(fmt::format("error parsing packet: expected header 0x{:X} but got 0x{:X}", 0xFE, b->operator[](0)));
    }

    b->erase(b->begin());

    if (encrypt) {
        encrypt->Decrypt(*b);
        encrypt->Verify(*b);
    }

    auto decompressor = ObjectPools::DecompressorPool.Get();
    auto decompressed = decompressor->Decompress(b);
    ObjectPools::DecompressorPool.Return(decompressor);

    while (decompressed->size() > 0) {
        auto packetSize = readVaruint32(decompressed);
        if (packetSize <= 0 || packetSize > decompressed->size()) {
            throw Exception(fmt::format("Invalid packet size {}", packetSize));
        }
        auto packetBuf = ObjectPools::ByteBufferPool.Get();
        packetBuf->resize(0);
        packetBuf->reserve(packetSize);
        packetBuf->insert(packetBuf->begin(), decompressed->begin(), decompressed->begin() + packetSize);
        decompressed->erase(decompressed->begin(), decompressed->begin() + packetSize);
        result.push_back(packetBuf);
    }

    ObjectPools::ByteBufferPool.Return(decompressed);

    return result;
}

void Decoder::EnableEncryption(const Common::ByteBuffer key) {
    encrypt = std::make_shared<Crypto::Minecrypt>(key);
}