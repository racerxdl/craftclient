#pragma once

#include <endian.h>
#include <server/asio/udp_client.h>

#include <asio.hpp>
#include <chrono>
#include <cstdint>
#include <exception>
#include <queue>
#include <string>
#include <vector>

namespace ProtoRock {
// typedef std::vector<uint8_t> PacketBuff;
// 24 bit unsigned int. It's actually a 32 bit, defined just to know where only 32 bit is used
typedef uint32_t uint24_t;
typedef std::vector<uint8_t> ByteBuffer;
typedef std::chrono::time_point<std::chrono::system_clock> Time;
typedef std::chrono::milliseconds TimeDuration;

inline ByteBuffer ByteBufferFromCBuffer(const void *buffer, size_t len) {
    ByteBuffer bb;
    bb.resize(len);
    memcpy(bb.data(), buffer, len);
    return bb;
}
inline Time Now() { return std::chrono::system_clock::now(); }

class PacketBuff : public std::deque<uint8_t> {
   public:
    PacketBuff() {}
    PacketBuff(const PacketBuff &b) : std::deque<uint8_t>(b) {}
    PacketBuff(const ByteBuffer &b) : std::deque<uint8_t>(b.begin(), b.end()) {}
    PacketBuff(const char *buff, int size) {
        for (int i = 0; i < size; i++) {
            Write(buff[i]);
        }
    }
    // Read Operations
    uint8_t Read();
    ByteBuffer ReadBytes(int);
    uint16_t ReadU16BE();
    uint32_t ReadU32BE();
    uint64_t ReadU64BE();
    int16_t ReadI16BE() { return (int16_t)ReadU16BE(); }
    int32_t ReadI32BE() { return (int32_t)ReadU32BE(); }
    int64_t ReadI64BE() { return (int64_t)ReadU16BE(); }
    uint24_t ReadUint24();

    uint16_t ReadU16LE();
    uint32_t ReadU32LE();
    uint64_t ReadU64LE();
    int16_t ReadI16LE() { return (int16_t)ReadU16LE(); }
    int32_t ReadI32LE() { return (int32_t)ReadU32LE(); }
    int64_t ReadI64LE() { return (int64_t)ReadU16LE(); }
    asio::ip::udp::endpoint ReadAddress();

    // Write Operations
    void Write(uint8_t b) { push_back(b); }
    void Write(const PacketBuff &b) { insert(end(), b.begin(), b.end()); }
    void Write(const ByteBuffer &b) { insert(end(), b.begin(), b.end()); }
    void Write(const asio::ip::udp::endpoint &a);
    void WriteBE(uint16_t v);
    void WriteBE(uint32_t v);
    void WriteBE(uint64_t v);
    void WriteBE(int16_t v) { WriteBE((uint16_t)v); }
    void WriteBE(int32_t v) { WriteBE((uint32_t)v); }
    void WriteBE(int64_t v) { WriteBE((uint64_t)v); }
    void WriteUint24(uint24_t v);

    void WriteLE(uint16_t v);
    void WriteLE(uint32_t v);
    void WriteLE(uint64_t v);
    void WriteLE(int16_t v) { WriteLE((uint16_t)v); }
    void WriteLE(int32_t v) { WriteLE((uint32_t)v); }
    void WriteLE(int64_t v) { WriteLE((uint64_t)v); }

    ByteBuffer ToBuffer() { return ByteBuffer(begin(), end()); }
};

class PacketBuffException : public std::exception {
   public:
    std::string msg;
    PacketBuffException(const std::string &msg) : msg(msg) {}
    virtual const char *what() const throw() {
        return msg.c_str();
    }
};

class RaknetException : public std::exception {
   public:
    std::string msg;
    RaknetException(const std::string &msg) : msg(msg) {}
    virtual const char *what() const throw() {
        return msg.c_str();
    }
};

typedef asio::ip::udp::endpoint UDPAddress;

inline int64_t timestamp() {
    return std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}
}  // namespace ProtoRock