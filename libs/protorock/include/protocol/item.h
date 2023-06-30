#pragma once

#include <fmt/format.h>

#include "common/common.h"
#include "protocol/packet/reader.h"
#include "protocol/packet/writer.h"

// https://github.com/Sandertv/gophertunnel/blob/master/minecraft/protocol/item.go

namespace ProtoRock {
namespace Protocol {

// ItemEntry is an item sent in the StartGame item table. It holds a name and a legacy ID, which is used to
// point back to that name.
struct ItemEntry {
    // Name if the name of the item, which is a name like 'minecraft:stick'.
    std::string Name;
    // RuntimeID is the ID that is used to identify the item over network. After sending all items in the
    // StartGame packet, items will then be identified using these numerical IDs.
    int16_t RuntimeId;
    // ComponentBased specifies if the item was created using components, meaning the item is a custom item.
    bool ComponentBased;

    void Serialize(Packet::Writer &w) const {
        w.Write(Name);
        w.WriteBE(RuntimeId);
        w.Write(ComponentBased);
    }
    void Deserialize(Packet::Reader &r) {
        r.Read(Name);
        r.ReadBE(RuntimeId);
        r.Read(ComponentBased);
    }
};

}  // namespace Protocol
}  // namespace ProtoRock