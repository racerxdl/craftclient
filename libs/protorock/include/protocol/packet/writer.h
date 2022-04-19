#pragma once

#include <math/vectors.h>

#include <cstdint>

#include "common/common.h"

namespace ProtoRock {
namespace Protocol {
namespace Packet {

class Writer {
   private:
    int32_t shieldId;
    std::shared_ptr<Common::PacketBuff> buff;

   public:
    Writer(std::shared_ptr<Common::PacketBuff> buff) : buff(buff) {}
    void SetShieldId(int32_t shieldId) { this->shieldId = shieldId; }
    void WriteFloat(const float v) { buff->WriteFloat(v); }
    void Write(const uint8_t v) { buff->Write(v); }
    void Write(const int8_t v) { Write((uint8_t)v); }
    void Write(const bool v) { Write((uint8_t)(v ? 1 : 0)); }
    void Write(const std::string v) {
        WriteVarint<uint32_t>(v.size());
        buff->Write(v);
    }
    void Write(const Common::ByteBuffer v) {
        WriteVarint<uint32_t>(v.size());
        buff->Write(v);
    }
    void WriteRaw(const Common::ByteBuffer v) { buff->Write(v); }
    void WriteByteFloat(const float v) { Write((uint8_t)(v / (360.0 / 256.0))); }
    void Write(const CraftBlock::Vec2 v) {
        WriteFloat(v.x);
        WriteFloat(v.y);
    }
    void Write(const CraftBlock::Vec3 v) {
        WriteFloat(v.x);
        WriteFloat(v.y);
        WriteFloat(v.z);
    }
    void Write(const CraftBlock::VecI2 v) {
        WriteVarint(v.x);
        WriteVarint(v.y);
    }
    void Write(const CraftBlock::VecI3 v) {
        WriteVarint(v.x);
        WriteVarint(v.y);
        WriteVarint(v.z);
    }
    void WriteRGBA(const CraftBlock::Vec4 v) {
        auto r = (uint32_t)(v[0] * 255.f);
        auto g = (uint32_t)(v[1] * 255.f);
        auto b = (uint32_t)(v[2] * 255.f);
        auto a = (uint32_t)(v[3] * 255.f);
        WriteVarint<uint32_t>(r | g << 8 | b << 16 | a << 24);
    }
    void Write(const Common::UUID uuid) {
        for (int i = 0; i < 16; i++) {
            buff->Write(uuid.value[i]);
        }
    }
    void WriteUTF(const std::string v) {
        buff->WriteLE((int16_t)v.size());
        buff->Write(v);
    }
    template <typename int_t = uint64_t>
    void WriteVarint(int_t v) {
        uint64_t ux = (uint64_t)v;
        if (v < 0) {
            ux = ~(ux << 1);
        }
        while (ux >= 0x80) {
            buff->Write((uint8_t)((ux & 0x7F) | 0x80));
            ux >>= 7;
        }
        buff->Write((uint8_t)(ux & 0x7F));
    }

    // Primitives
    void WriteBE(const int16_t v) { buff->WriteBE(v); }
    void WriteBE(const int32_t v) { buff->WriteBE(v); }
    void WriteBE(const int64_t v) { buff->WriteBE(v); }
    void WriteBE(const uint16_t v) { buff->WriteBE(v); }
    void WriteBE(const uint32_t v) { buff->WriteBE(v); }
    void WriteBE(const uint64_t v) { buff->WriteBE(v); }

    void WriteLE(const int16_t v) { buff->WriteLE(v); }
    void WriteLE(const int32_t v) { buff->WriteLE(v); }
    void WriteLE(const int64_t v) { buff->WriteLE(v); }
    void WriteLE(const uint16_t v) { buff->WriteLE(v); }
    void WriteLE(const uint32_t v) { buff->WriteLE(v); }
    void WriteLE(const uint64_t v) { buff->WriteLE(v); }
};

}  // namespace Packet
}  // namespace Protocol
}  // namespace ProtoRock