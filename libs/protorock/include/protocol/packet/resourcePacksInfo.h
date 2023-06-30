#pragma once

// https://github.com/Sandertv/gophertunnel/blob/master/minecraft/protocol/packet/client_cache_status.go

#include "protocol/packet/packet.h"
#include "protocol/resourcePack.h"

namespace ProtoRock {
namespace Protocol {
namespace Packet {

// ResourcePacksInfo is sent by the server to inform the client on what resource packs the server has. It
// sends a list of the resource packs it has and basic information on them like the version and description.
struct ResourcePacksInfo : public Packet {
    bool TexturePackRequired = false;
    bool HasScripts = false;

    std::vector<BehaviourPackInfo> BehaviourPacks;
    std::vector<TexturePackInfo> TexturePacks;
    bool ForcingServerPacks = false;

    bool IsRaw() const override { return false; }
    PacketID Id() const override { return PacketID::ResourcePacksInfo; }
    virtual void Serialize(Writer &w) const override {
        w.Write(TexturePackRequired);
        w.Write(HasScripts);
        w.Write(ForcingServerPacks);
        w.WriteLE((uint16_t)BehaviourPacks.size());
        for (auto &p : BehaviourPacks) {
            p.Serialize(w);
        }
        w.WriteLE((uint16_t)TexturePacks.size());
        for (auto &p : TexturePacks) {
            p.Serialize(w);
        }
    };
    virtual void Deserialize(Reader &r) override {
        r.Read(TexturePackRequired);
        r.Read(HasScripts);
        r.Read(ForcingServerPacks);
        int16_t s;
        r.ReadLE(s);
        for (int i = 0; i < s; i++) {
            BehaviourPackInfo p;
            p.Deserialize(r);
            BehaviourPacks.push_back(p);
        }
        r.ReadLE(s);
        for (int i = 0; i < s; i++) {
            TexturePackInfo p;
            p.Deserialize(r);
            TexturePacks.push_back(p);
        }
    }
};
}  // namespace Packet
}  // namespace Protocol
}  // namespace ProtoRock