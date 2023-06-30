#pragma once

// https://github.com/Sandertv/gophertunnel/blob/master/minecraft/protocol/packet/client_cache_status.go

#include <string>
#include <vector>

#include "protocol/experimentData.h"
#include "protocol/packet/packet.h"
#include "protocol/resourcePack.h"

namespace ProtoRock {
namespace Protocol {
namespace Packet {

// ResourcePackStack is sent by the server to send the order in which resource packs and behaviour packs
// should be applied (and downloaded) by the client.
struct ResourcePackStack : public Packet {
    // TexturePackRequired specifies if the client must accept the texture packs the server has in order to
    // join the server. If set to true, the client gets the option to either download the resource packs and
    // join, or quit entirely. Behaviour packs never have to be downloaded.
    bool TexturePackRequired = false;
    // BehaviourPack is a list of behaviour packs that the client needs to download before joining the server.
    // All of these behaviour packs will be applied together, and the order does not necessarily matter.
    std::vector<Protocol::StackResourcePack> BehaviourPacks;
    // TexturePacks is a list of texture packs that the client needs to download before joining the server.
    // The order of these texture packs specifies the order that they are applied in on the client side. The
    // first in the list will be applied first.
    std::vector<Protocol::StackResourcePack> TexturePacks;
    // BaseGameVersion is the vanilla version that the client should set its resource pack stack to.
    std::string BaseGameVersion;
    // Experiments holds a list of experiments that are either enabled or disabled in the world that the
    // player spawns in.
    // It is not clear why experiments are sent both here and in the StartGame packet.
    std::vector<Protocol::ExperimentData> Experiments;
    // ExperimentsPreviouslyToggled specifies if any experiments were previously toggled in this world. It is
    // probably used for some kind of metrics.
    bool ExperimentsPreviouslyToggled = false;

    bool IsRaw() const override { return false; }
    PacketID Id() const override { return PacketID::ResourcePackStack; }
    virtual void Serialize(Writer &w) const override {
        w.Write(TexturePackRequired);
        w.WriteVarint<uint32_t>(BehaviourPacks.size());
        for (auto &s : BehaviourPacks) {
            s.Serialize(w);
        }
        w.WriteVarint<uint32_t>(TexturePacks.size());
        for (auto &s : TexturePacks) {
            s.Serialize(w);
        }
        w.Write(BaseGameVersion);
        w.WriteLE(Experiments.size());
        for (auto &e : Experiments) {
            e.Serialize(w);
        }
        w.Write(ExperimentsPreviouslyToggled);
    }
    virtual void Deserialize(Reader &r) override {
        r.Read(TexturePackRequired);
        uint32_t l;
        r.ReadVarint<uint32_t>(l);
        for (int i = 0; i < l; i++) {
            StackResourcePack p;
            p.Deserialize(r);
            BehaviourPacks.push_back(p);
        }
        r.ReadVarint<uint32_t>(l);
        for (int i = 0; i < l; i++) {
            StackResourcePack p;
            p.Deserialize(r);
            TexturePacks.push_back(p);
        }
        r.Read(BaseGameVersion);
        r.ReadLE(l);
        for (int i = 0; i < l; i++) {
            ExperimentData p;
            p.Deserialize(r);
            Experiments.push_back(p);
        }
        r.Read(ExperimentsPreviouslyToggled);
    }
};
}  // namespace Packet
}  // namespace Protocol
}  // namespace ProtoRock