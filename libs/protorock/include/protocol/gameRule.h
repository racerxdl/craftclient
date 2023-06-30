#pragma once

#include <fmt/format.h>

#include "common/common.h"
#include "protocol/packet/reader.h"
#include "protocol/packet/writer.h"

// https://github.com/Sandertv/gophertunnel/blob/master/minecraft/protocol/game_rule.go

namespace ProtoRock {
namespace Protocol {
struct GameRule {
    std::string Name;
    bool CanBeModifiedByPlayer;
    uint32_t valueType;
    union {
        bool booleanValue;
        uint32_t uint32Value;
        float floatValue;
    };

    void Serialize(Packet::Writer &w) const {
        w.Write(Name);
        w.Write(CanBeModifiedByPlayer);
        w.WriteVarint<uint32_t>(valueType);
        switch (valueType) {
            case 0:
                w.Write(booleanValue);
                break;
            case 1:
                w.WriteVarint<uint32_t>(uint32Value);
                break;
            case 2:
                w.WriteFloat(floatValue);
                break;
            default:
                throw Common::Exception(fmt::format("invalid valueType {}", valueType));
        }
    };
    void Deserialize(Packet::Reader &r) {
        r.Read(Name);
        r.Read(CanBeModifiedByPlayer);
        r.ReadVarint<uint32_t>(valueType);
        switch (valueType) {
            case 0:
                r.Read(booleanValue);
                break;
            case 1:
                r.ReadVarint<uint32_t>(uint32Value);
                break;
            case 2:
                r.ReadFloat(floatValue);
                break;
            default:
                throw Common::Exception(fmt::format("invalid valueType {}", valueType));
        }
    }
};
}  // namespace Protocol
}  // namespace ProtoRock