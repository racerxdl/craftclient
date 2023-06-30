#pragma once

// https://github.com/Sandertv/gophertunnel/blob/master/minecraft/protocol/packet/server_to_client_handshake.go

#include "protocol/packet/packet.h"

namespace ProtoRock {
namespace Protocol {
namespace Packet {

// ServerToClientHandshake is sent by the server to the client to complete the key exchange in order to
// initialise encryption on client and server side. It is followed up by a ClientToServerHandshake packet
// from the client.
struct ServerToClientHandshake : public Packet {
    // JWT is a raw JWT token containing data such as the public key from the server, the algorithm used and
    // the server's token. It is used for the client to produce a shared secret.
    Common::ByteBuffer JWT;

    bool IsRaw() const override { return false; }
    PacketID Id() const override { return PacketID::ServerToClientHandshake; }
    virtual void Serialize(Writer &w) const override {
        w.Write(JWT);
    };
    virtual void Deserialize(Reader &r) override {
        r.Read(JWT);
    }
};
}  // namespace Packet
}  // namespace Protocol
}  // namespace ProtoRock