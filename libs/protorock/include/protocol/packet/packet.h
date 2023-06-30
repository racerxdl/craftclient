#pragma once

#include <cstdint>
#include <memory>
#include <list>

#include "protocol/packet/id.h"
#include "protocol/packet/reader.h"
#include "protocol/packet/writer.h"

namespace ProtoRock {
namespace Protocol {
namespace Packet {

struct Packet {
    virtual PacketID Id() const = 0;
    virtual bool IsRaw() const = 0;
    virtual void Serialize(Writer &w) const = 0;
    virtual void Deserialize(Reader &r) = 0;

    template <typename T, typename std::enable_if<std::is_base_of<Packet, T>::value>::type* = nullptr>
    T as() { return *dynamic_cast<T*>(this); }
};

struct PacketHeader {
    PacketID packetId;
    uint8_t SenderSubClient = 0;
    uint8_t TargetSubClient = 0;
    void Serialize(Writer &w) const {
        w.WriteVarint((uint32_t)packetId | ((uint32_t)SenderSubClient) << 10 | ((uint32_t)TargetSubClient) << 12);
    }
    void Deserialize(Reader &r) {
        uint32_t v;
        r.ReadVarint(v);
        packetId = (PacketID)(v & 0x3FF);
        SenderSubClient = (uint8_t)((v >> 10) & 3);
        TargetSubClient = (uint8_t)((v >> 12) & 3);
    }
};

// Raw packet for unknown packets
struct RawPacket : public Packet {
    PacketID id;
    Common::ByteBuffer payload;

    RawPacket(PacketID id) : id(id) {}
    bool IsRaw() const override { return true; }
    PacketID Id() const override { return id; }
    void Serialize(Writer &w) const override { w.WriteRaw(payload); };
    void Deserialize(Reader &r) override { r.ReadRaw(payload); }
};

typedef std::shared_ptr<Packet> IPacket;
typedef std::function<IPacket()> PacketCreateFunction;
using PacketList = std::list<IPacket>;

}  // namespace Packet
}  // namespace Protocol
}  // namespace ProtoRock