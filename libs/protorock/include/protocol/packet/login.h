#pragma once

// https://github.com/Sandertv/gophertunnel/blob/master/minecraft/protocol/packet/login.go

#include "protocol/packet/packet.h"

namespace ProtoRock {
namespace Protocol {
namespace Packet {

// Login is sent when the client initially tries to join the server. It is the first packet sent and contains
// information specific to the player.
struct Login : public Packet {
    // ClientProtocol is the protocol version of the player. The player is disconnected if the protocol is
    // incompatible with the protocol of the server.
    int32_t ClientProtocol;
    // ConnectionRequest is a string containing information about the player and JWTs that may be used to
    // verify if the player is connected to XBOX Live. The connection request also contains the necessary
    // client public key to initiate encryption.
    Common::ByteBuffer ConnectionRequest;

    bool IsRaw() const override { return false; }
    PacketID Id() const override { return PacketID::Login; }
    virtual void Serialize(Writer &w) const override {
        w.WriteBE(ClientProtocol);
        w.Write(ConnectionRequest);
    };
    virtual void Deserialize(Reader &r) override {
        r.ReadBE(ClientProtocol);
        r.Read(ConnectionRequest);
    }
};
}  // namespace Packet
}  // namespace Protocol
}  // namespace ProtoRock