#pragma once

// https://github.com/Sandertv/gophertunnel/blob/master/minecraft/protocol/packet/server_to_client_handshake.go

#include "protocol/packet/packet.h"

namespace ProtoRock {
namespace Protocol {
namespace Packet {

enum struct PlayStatusCode : int32_t {
	LoginSuccess = 0,
	LoginFailedClient,
	LoginFailedServer,
	PlayerSpawn,
	LoginFailedInvalidTenant,
	LoginFailedVanillaEdu,
	LoginFailedEduVanilla,
	LoginFailedServerFull
};

// PlayStatus is sent by the server to update a player on the play status. This includes failed statuses due
// to a mismatched version, but also success statuses.
struct PlayStatus : public Packet {
    PlayStatusCode Status;

    bool IsRaw() const override { return false; }
    PacketID Id() const override { return PacketID::PlayStatus; }
    virtual void Serialize(Writer &w) const override {
        w.WriteBE((int32_t)Status);
    };
    virtual void Deserialize(Reader &r) override {
        int32_t v = 0;
        r.ReadBE(v);
        Status = (PlayStatusCode)v;
    }
};
}  // namespace Packet
}  // namespace Protocol
}  // namespace ProtoRock

