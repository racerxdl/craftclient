#pragma once

#include <fmt/format.h>

#include "common/common.h"
#include "protocol/packet/reader.h"
#include "protocol/packet/writer.h"

// https://github.com/Sandertv/gophertunnel/blob/master/minecraft/protocol/player.go

namespace ProtoRock {
namespace Protocol {

// PlayerMovementSettings represents the different server authoritative movement settings. These control how
// the client will provide input to the server.
struct PlayerMovementSettings {
    // MovementType specifies the way the server handles player movement. Available options are
    // packet.AuthoritativeMovementModeClient, packet.AuthoritativeMovementModeServer and
    // packet.AuthoritativeMovementModeServerWithRewind, where the server authoritative types result
    // in the client sending PlayerAuthInput packets instead of MovePlayer packets and the rewind mode
    // requires sending the tick of movement and several actions.
    int32_t MovementType;
    // RewindHistorySize is the amount of history to keep at maximum if MovementType is
    // packet.AuthoritativeMovementModeServerWithRewind.
    int32_t RewindHistorySize;
    // ServerAuthoritativeBlockBreaking specifies if block breaking should be sent through
    // packet.PlayerAuthInput or not. This field is somewhat redundant as it is always enabled if
    // MovementType is packet.AuthoritativeMovementModeServer or
    // packet.AuthoritativeMovementModeServerWithRewind
    bool ServerAuthoritativeBlockBreaking;

    void Serialize(Packet::Writer &w) const {
        w.WriteVarint<uint32_t>(MovementType);
        w.WriteVarint<uint32_t>(RewindHistorySize);
        w.Write(ServerAuthoritativeBlockBreaking);
    };
    void Deserialize(Packet::Reader &r) {
        r.ReadVarint<uint32_t>(MovementType);
        r.ReadVarint<uint32_t>(RewindHistorySize);
        r.Read(ServerAuthoritativeBlockBreaking);
    }
};

}  // namespace Protocol
}  // namespace ProtoRock