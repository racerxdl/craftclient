#pragma once

#include <string>

#include "protocol/error.h"

namespace ProtoRock {
namespace Protocol {

// https://github.com/Sandertv/gophertunnel/blob/master/minecraft/protocol/login/data.go#L18

// IdentityData contains identity data of the player logged in. It is found in one of the JWT claims signed
// by Mojang, and can thus be trusted.
struct IdentityData {
    // XUID is the XBOX Live user ID of the player, which will remain consistent as long as the player is
    // logged in with the XBOX Live account. It is empty if the user is not logged into its XBL account.
    std::string XUID;
    // Identity is the UUID of the player, which will also remain consistent for as long as the user is logged
    // into its XBOX Live account.
    std::string Identity;
    // DisplayName is the username of the player, which may be changed by the user. It should for that reason
    // not be used as a key to store information.
    std::string DisplayName;
    // TitleID is a numerical ID present only if the user is logged into XBL. It holds the title ID (XBL
    // related) of the version that the player is on. Some of these IDs may be found below.
    // Win10: 896928775
    // Mobile: 1739947436
    // Nintendo: 2047319603
    // Note that these IDs are protected using XBOX Live, making the spoofing of this data very difficult.
    std::string TitleId;

    IdentityData();
    Error Validate();
    void Clear() { XUID = ""; TitleId = ""; }
};
}  // namespace Protocol
}  // namespace ProtoRock