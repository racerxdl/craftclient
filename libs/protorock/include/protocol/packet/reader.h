#pragma once

#include <math/vectors.h>

#include <cstdint>

#include "common/common.h"

namespace ProtoRock {
namespace Protocol {
namespace Packet {

class Reader {
   private:
    int32_t shieldId;
    std::shared_ptr<Common::PacketBuff> buff;

   public:
    Reader(std::shared_ptr<Common::PacketBuff> buff) : buff(buff) {}
    void Read(uint8_t &v) { v = buff->Read(); }
    void Read(int8_t &v) { v = (uint8_t)buff->Read(); }
    void Read(bool &v) { v = buff->Read() ? true : false; }
    void ReadFloat(float &v) { v = buff->ReadFloat(); }
    void Read(std::string &s) {
        uint32_t length;
        ReadVarint(length);
        if (length > UINT16_MAX) {
            throw Common::Exception("string read too big");
        }
        s.resize(length);
        for (int i = 0; i < length; i++) {
            s[i] = buff->Read();
        }
    }
    void Read(Common::ByteBuffer &v) {
        uint32_t length;
        ReadVarint(length);
        v.resize(length);
        buff->ReadBytes(v, length);
    }
    void Read(CraftBlock::Vec3 &v) {
        ReadFloat(v.x);
        ReadFloat(v.y);
        ReadFloat(v.z);
    }
    void Read(CraftBlock::Vec2 &v) {
        ReadFloat(v.x);
        ReadFloat(v.y);
    }
    void Read(CraftBlock::VecI3 &v) {
        ReadVarint(v[0]);
        ReadVarint(v[1]);
        ReadVarint(v[2]);
    }
    void Read(CraftBlock::VecI2 &v) {
        ReadVarint(v[0]);
        ReadVarint(v[1]);
    }
    void ReadRGBA(CraftBlock::Vec4 &v) {
        uint32_t o;
        ReadVarint(o);
        v.r = (o & 0xFF) / 255.f;
        v.g = ((o >> 8) & 0xFF) / 255.f;
        v.b = ((o >> 16) & 0xFF) / 255.f;
        v.a = ((o >> 24) & 0xFF) / 255.f;
    }
    void Read(Common::UUID &uuid) {
        buff->ReadBytes((char *)uuid.value, 16);
        throw Common::Exception("TODO");
    }

    template <typename int_t = uint64_t>
    void ReadVarint(int_t &v) {
        constexpr int count = sizeof(int_t) == 8 ? 70 : 35;
        v = 0;
        for (int i = 0; i < count; i+=7) {
            auto b = buff->Read();
            v |= (int_t)(b & 0x7F) << i;
            if ((b & 0x80) == 0) {
                return;
            }
        }
        throw Common::Exception("varint overflow");
    }

    void ReadBE(int16_t &v) { v = buff->ReadI16BE(); }
    void ReadBE(int32_t &v) { v = buff->ReadI32BE(); }
    void ReadBE(int64_t &v) { v = buff->ReadI64BE(); }
    void ReadBE(uint16_t &v) { v = buff->ReadU16BE(); }
    void ReadBE(uint32_t &v) { v = buff->ReadU32BE(); }
    void ReadBE(uint64_t &v) { v = buff->ReadU64BE(); }

    void ReadLE(int16_t &v) { v = buff->ReadI16LE(); }
    void ReadLE(int32_t &v) { v = buff->ReadI32LE(); }
    void ReadLE(int64_t &v) { v = buff->ReadI64LE(); }
    void ReadLE(uint16_t &v) { v = buff->ReadU16LE(); }
    void ReadLE(uint32_t &v) { v = buff->ReadU32LE(); }
    void ReadLE(uint64_t &v) { v = buff->ReadU64LE(); }

};
}  // namespace Packet
}  // namespace Protocol
}  // namespace ProtoRock