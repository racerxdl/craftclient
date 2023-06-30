#pragma once

// https://github.com/Sandertv/gophertunnel/blob/master/minecraft/protocol/packet/client_cache_status.go

#include <string>
#include <vector>

#include "protocol/packet/packet.h"

namespace ProtoRock {
namespace Protocol {
namespace Packet {

enum struct PackResponseCode : uint8_t {
    Refused = 1,
    SendPacks,
    AllPacksDownloaded,
    Completed,
};

// ResourcePackClientResponse is sent by the client in response to resource packets sent by the server. It is
// used to let the server know what action needs to be taken for the client to have all resource packs ready
// and set.
struct ResourcePackClientResponse : public Packet {
    // Response is the response type of the response. It is one of the constants found above.
    PackResponseCode Response;
    // PacksToDownload is a list of resource pack UUIDs combined with their version that need to be downloaded
    // (for example SomePack_1.0.0), if the Response field is PackResponseSendPacks.
    std::vector<std::string> PacksToDownload;

    ResourcePackClientResponse() {}
    ResourcePackClientResponse(PackResponseCode code) : Response(code) {}
    bool IsRaw() const override { return false; }
    PacketID Id() const override { return PacketID::ResourcePackClientResponse; }
    virtual void Serialize(Writer &w) const override {
        w.Write((uint8_t)Response);
        w.WriteLE((uint16_t)PacksToDownload.size());
        for (auto p : PacksToDownload) {
            w.Write(p);
        }
    };
    virtual void Deserialize(Reader &r) override {
        uint8_t v;
        r.Read(v);
        Response = (PackResponseCode)v;
        uint16_t count;
        r.ReadLE(count);
        for (int i = 0; i < count; i++) {
            std::string v;
            r.Read(v);
            PacksToDownload.push_back(v);
        }
    }
};
}  // namespace Packet
}  // namespace Protocol
}  // namespace ProtoRock