#pragma once

#include <fmt/format.h>

#include "common/common.h"
#include "protocol/packet/reader.h"
#include "protocol/packet/writer.h"

// https://github.com/Sandertv/gophertunnel/blob/master/minecraft/protocol/block.go

namespace ProtoRock {
namespace Protocol {

// BlockEntry is an entry for a custom block found in the StartGame packet. The runtime ID of these custom
// block entries is based on the index they have in the block palette when the palette is ordered
// alphabetically.
struct BlockEntry {
    // Name is the name of the custom block.
    std::string Name;
    // Properties is a list of properties which, in combination with the name, specify a unique block.
    Common::ByteBuffer Properties;  // TODO NBT

    void Serialize(Packet::Writer &w) const {
        w.Write(Name);
        w.WriteRaw(Properties);
        throw Common::Exception("IMPLEMENT NBT");
    }
    void Deserialize(Packet::Reader &r) {
        r.Read(Name);
        r.ReadRaw(Properties);
        throw Common::Exception("IMPLEMENT NBT");
    }
};

}  // namespace Protocol
}  // namespace ProtoRock