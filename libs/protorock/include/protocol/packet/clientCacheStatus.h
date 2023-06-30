#pragma once

// https://github.com/Sandertv/gophertunnel/blob/master/minecraft/protocol/packet/client_cache_status.go

#include "protocol/packet/packet.h"

namespace ProtoRock {
namespace Protocol {
namespace Packet {

// ClientCacheStatus is sent by the client to the server at the start of the game. It is sent to let the
// server know if it supports the client-side blob cache. Clients such as Nintendo Switch do not support the
// cache, and attempting to use it anyway will fail.
struct ClientCacheStatus : public Packet {
	// Enabled specifies if the blob cache is enabled. If false, the server should not attempt to use the
	// blob cache. If true, it may do so, but it may also choose not to use it.
    bool Enabled = false;

    bool IsRaw() const override { return false; }
    PacketID Id() const override { return PacketID::ClientCacheStatus; }
    virtual void Serialize(Writer &w) const override{ w.Write(Enabled); };
    virtual void Deserialize(Reader &r) override { r.Read(Enabled); }
};
}  // namespace Packet
}  // namespace Protocol
}  // namespace ProtoRock