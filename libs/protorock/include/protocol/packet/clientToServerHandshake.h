#pragma once

// https://github.com/Sandertv/gophertunnel/blob/master/minecraft/protocol/packet/client_to_server_handshake.go

#include "protocol/packet/packet.h"

namespace ProtoRock {
namespace Protocol {
namespace Packet {

// ClientToServerHandshake is sent by the client in response to a ServerToClientHandshake packet sent by the
// server. It is the first encrypted packet in the login handshake and serves as a confirmation that
// encryption is correctly initialised client side.
struct ClientToServerHandshake : public Packet {
    // ClientToServerHandshake has no fields.

    bool IsRaw() const override { return false; }
    PacketID Id() const override { return PacketID::ClientToServerHandshake; }
    virtual void Serialize(Writer &w) const override{};
    virtual void Deserialize(Reader &r) override {}
};
}  // namespace Packet
}  // namespace Protocol
}  // namespace ProtoRock