#pragma once

#include <cstdint>

#include "protocol/packet/id.h"
#include "protocol/packet/reader.h"
#include "protocol/packet/writer.h"

namespace ProtoRock {
namespace Protocol {
namespace Packet {

struct Packet {
    virtual PacketID Id() const = 0;
    virtual void Serialize(Writer &w) const = 0;
    virtual void Deserialize(Reader &r) = 0;
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

}  // namespace Packet
}  // namespace Protocol
}  // namespace ProtoRock