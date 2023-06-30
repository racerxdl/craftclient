#pragma once

#include "common/common.h"
#include "protocol/packet/reader.h"
#include "protocol/packet/writer.h"

// https://github.com/Sandertv/gophertunnel/blob/master/minecraft/protocol/education.go

namespace ProtoRock {
namespace Protocol {
// EducationSharedResourceURI is an education edition feature that is used for transmitting
// education resource settings to clients. It contains a button name and a link URL.
struct EducationSharedResourceURI {
    // ButtonName is the button name of the resource URI.
    std::string ButtonName;
    // LinkURI is the link URI for the resource URI.
    std::string LinkURI;

    void Serialize(Packet::Writer &w) const {
        w.Write(ButtonName);
        w.Write(LinkURI);
    };
    void Deserialize(Packet::Reader &r) {
        r.Read(ButtonName);
        r.Read(LinkURI);
    }
};

// EducationSharedResourceURI is an education edition feature that is used for transmitting
// education resource settings to clients. It contains a button name and a link URL.
struct EducationExternalLinkSettings {
    // URL is the external link URL.
    std::string URL;
    // DisplayName is the display name in game.
    std::string DisplayName;

    void Serialize(Packet::Writer &w) const {
        w.Write(URL);
        w.Write(DisplayName);
    };
    void Deserialize(Packet::Reader &r) {
        r.Read(URL);
        r.Read(DisplayName);
    }
};

}  // namespace Protocol
}  // namespace ProtoRock