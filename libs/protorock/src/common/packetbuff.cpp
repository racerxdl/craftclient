#include "common/common.h"

namespace ProtoRock {
namespace Common {

uint24_t PacketBuff::ReadUint24() {
    auto a = (uint24_t)Read();
    auto b = (uint24_t)Read();
    auto c = (uint24_t)Read();

    return a | (b << 8) | (c << 16);
}

uint8_t PacketBuff::Read() {
    if (!size()) {
        throw PacketBuffException("no enough bytes");
    }
    uint8_t v = front();
    pop_front();
    return v;
}
void PacketBuff::ReadBytes(char *b, int n) {
    if (n > size()) {
        throw PacketBuffException("no enough bytes");
    }
    for (int i = 0; i < n; i++) {
        b[i] = *(begin()+i);
    }
    erase(begin(), begin()+n);
}
void PacketBuff::ReadBytes(ByteBuffer &b, int n) {
    if (n > b.size()) {
        throw PacketBuffException("no enough space in target buffer");
    }
    if (n > size()) {
        throw PacketBuffException("no enough bytes");
    }

    for (int i = 0; i < n; i++) {
        b[i] = *(begin()+i);
    }
    erase(begin(), begin()+n);
}

ByteBuffer PacketBuff::ReadBytes(int n) {
    auto start = begin();
    if (n > size()) {
        throw PacketBuffException("no enough bytes");
    }
    auto end = start + n;
    auto v = ByteBuffer(start, end);
    erase(start, end);
    return v;
}

uint16_t PacketBuff::ReadU16BE() {
    uint16_t v;
    uint8_t *o = (uint8_t *)&v;
    o[0] = Read();
    o[1] = Read();

    v = be16toh(v);

    return v;
}

uint32_t PacketBuff::ReadU32BE() {
    uint32_t v;
    uint8_t *o = (uint8_t *)&v;
    o[0] = Read();
    o[1] = Read();
    o[2] = Read();
    o[3] = Read();

    v = be32toh(v);

    return v;
}

uint64_t PacketBuff::ReadU64BE() {
    uint64_t v;
    uint8_t *o = (uint8_t *)&v;
    o[0] = Read();
    o[1] = Read();
    o[2] = Read();
    o[3] = Read();
    o[4] = Read();
    o[5] = Read();
    o[6] = Read();
    o[7] = Read();

    v = be64toh(v);

    return v;
}

void PacketBuff::WriteBE(uint16_t v) {
    v = htobe16(v);
    const uint8_t *o = (uint8_t *)&v;
    push_back(o[0]);
    push_back(o[1]);
}

void PacketBuff::WriteBE(uint32_t v) {
    v = htobe32(v);
    const uint8_t *o = (uint8_t *)&v;
    push_back(o[0]);
    push_back(o[1]);
    push_back(o[2]);
    push_back(o[3]);
}
void PacketBuff::WriteBE(uint64_t v) {
    v = htobe64(v);
    const uint8_t *o = (uint8_t *)&v;
    push_back(o[0]);
    push_back(o[1]);
    push_back(o[2]);
    push_back(o[3]);
    push_back(o[4]);
    push_back(o[5]);
    push_back(o[6]);
    push_back(o[7]);
}

void PacketBuff::WriteUint24(uint24_t v) {
    push_back((uint8_t)(v & 0xFF));
    push_back((uint8_t)((v >> 8) & 0xFF));
    push_back((uint8_t)((v >> 16) & 0xFF));
}

void PacketBuff::WriteLE(uint16_t v) {
    v = htole16(v);
    const uint8_t *o = (uint8_t *)&v;
    push_back(o[0]);
    push_back(o[1]);
}

void PacketBuff::WriteLE(uint32_t v) {
    v = htole32(v);
    const uint8_t *o = (uint8_t *)&v;
    push_back(o[0]);
    push_back(o[1]);
    push_back(o[2]);
    push_back(o[3]);
}

void PacketBuff::WriteLE(uint64_t v) {
    v = htole64(v);
    const uint8_t *o = (uint8_t *)&v;
    push_back(o[0]);
    push_back(o[1]);
    push_back(o[2]);
    push_back(o[3]);
    push_back(o[4]);
    push_back(o[5]);
    push_back(o[6]);
    push_back(o[7]);
}

uint16_t PacketBuff::ReadU16LE() {
    uint16_t v;
    uint8_t *o = (uint8_t *)&v;
    o[0] = Read();
    o[1] = Read();

    v = le16toh(v);

    return v;
}

uint32_t PacketBuff::ReadU32LE() {
    uint32_t v;
    uint8_t *o = (uint8_t *)&v;
    o[0] = Read();
    o[1] = Read();
    o[2] = Read();
    o[3] = Read();

    v = le32toh(v);

    return v;
}

uint64_t PacketBuff::ReadU64LE() {
    uint64_t v;
    uint8_t *o = (uint8_t *)&v;
    o[0] = Read();
    o[1] = Read();
    o[2] = Read();
    o[3] = Read();
    o[4] = Read();
    o[5] = Read();
    o[6] = Read();
    o[7] = Read();

    v = le64toh(v);

    return v;
}

asio::ip::udp::endpoint PacketBuff::ReadAddress() {
    asio::ip::udp::endpoint e;
    auto ver = Read();

    if (ver == 4) {
        asio::detail::array<unsigned char, 4> ipbytes;
        ipbytes[0] = ~Read();
        ipbytes[1] = ~Read();
        ipbytes[2] = ~Read();
        ipbytes[3] = ~Read();
        auto port = ReadI16BE();
        e = asio::ip::udp::endpoint(asio::ip::address_v4(ipbytes), port);
    } else {
        ReadI16BE();  // Discard
        auto port = ReadI16BE();
        asio::detail::array<unsigned char, 16> ipbytes;
        for (int i = 0; i < 16; i++) {
            ipbytes[i] = Read();
        }
        e = asio::ip::udp::endpoint(asio::ip::address_v6(ipbytes), port);
    }

    return e;
}

void PacketBuff::Write(const asio::ip::udp::endpoint &a) {
    auto ver = a.address().is_v4() ? 4 : 6;
    auto addr = a.address();
    Write((uint8_t)ver);

    if (ver == 4) {
        auto v4 = addr.to_v4();
        auto ipbytes = v4.to_bytes();
        Write(~ipbytes[0]);
        Write(~ipbytes[1]);
        Write(~ipbytes[2]);
        Write(~ipbytes[3]);
        WriteBE((uint16_t)a.port());
    } else {
        auto v6 = addr.to_v6();
        auto ipbytes = v6.to_bytes();
        auto port = a.port();
        if (addr.is_unspecified()) {
            memset(ipbytes.data(), 0, 16);
            port = 0;
        }
        WriteLE((uint16_t)23);  // AF_INET6 on Windows
        WriteBE((uint16_t)port);
        WriteBE((uint32_t)0);
        for (int i = 0; i < 16; i++) {
            Write(ipbytes[i]);
        }
        WriteBE((uint32_t)0);
    }
}

void PacketBuff::WriteFloat(float f) {
    auto b = (char *)&f;
    Write(b[0]);
    Write(b[1]);
    Write(b[2]);
    Write(b[3]);
}

float PacketBuff::ReadFloat() {
    float f;
    auto b = (char *)&f;
    b[0] = Read();
    b[1] = Read();
    b[2] = Read();
    b[3] = Read();
    return f;
}

}  // namespace Common
}  // namespace ProtoRock