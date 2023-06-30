#pragma once

// https://github.com/Sandertv/gophertunnel/blob/master/minecraft/protocol/packet/client_cache_status.go

#include "protocol/packet/packet.h"

namespace ProtoRock {
namespace Protocol {
namespace Packet {

// SetLocalPlayerAsInitialised is sent by the client in response to a PlayStatus packet with the status set
// to 3. The packet marks the moment at which the client is fully initialised and can receive any packet
// without discarding it.
struct SetLocalPlayerAsInitialised : public Packet {
	// EntityRuntimeID is the entity runtime ID the player was assigned earlier in the login sequence in the
	// StartGame packet.
    uint64_t EntityRuntimeId = 0;

    bool IsRaw() const override { return false; }
    PacketID Id() const override { return PacketID::SetLocalPlayerAsInitialised; }
    virtual void Serialize(Writer &w) const override{ w.WriteVarint<uint64_t>(EntityRuntimeId); };
    virtual void Deserialize(Reader &r) override { r.ReadVarint<uint64_t>(EntityRuntimeId); }
};
}  // namespace Packet
}  // namespace Protocol
}  // namespace ProtoRock