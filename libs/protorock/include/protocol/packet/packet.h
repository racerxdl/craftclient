#pragma once

#include <cstdint>

namespace ProtoRock {
namespace Protocol {
namespace Packet {

class Packet {
    public:
    virtual uint32_t Id() = 0;
    virtual void Serialize() = 0;
    virtual void Deserialize() = 0;
};

}  // namespace Packet
}  // namespace Protocol
}  // namespace ProtoRock