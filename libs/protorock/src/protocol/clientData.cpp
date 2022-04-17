#include "protocol/clientData.h"

#include <regex>
#include <stdexcept>

#include <fmt/core.h>
#include <asio.hpp>
#include <system/uuid.h>
#include <jsoncpp/json/json.h>

#include "protocol.h"
#include "protocol/validate.h"
#include "common/base64.h"
#include "resources/skin.h"

using namespace ProtoRock;
using namespace ProtoRock::Protocol;


Error ClientData::Validate() {
    if (OS <= 0 || OS > 13) {
        return Error(fmt::format("DeviceOS must carry a value between 1 and 13, but got {}", OS));
    }

    if (!Validate::UUID(DeviceID)) {
        return Error(fmt::format("DeviceID must be parseable as a valid UUID, but go {}", DeviceID));
    }

    if (!Validate::Version(GameVersion)) {
        return Error(fmt::format("GameVersion must only contain dots and numbers, but got {}", GameVersion));
    }

    // TODO: Validate BCP-47 ISO language code field language

    if (PlatformOfflineId.size() != 0 && !Validate::UUID(PlatformOnlineId)) {
        return Error(fmt::format("PlatformOfflineId must be parseable as a valid UUID or empty, but got {}", PlatformOfflineId));
    }

    if (PlatformOnlineId.size() != 0 && !Validate::StringToInt(PlatformOnlineId)) {
        return Error(fmt::format("PlatformOnlineId must be parseable as an int64 or empty, but got {}", PlatformOfflineId));
    }

    if (!Validate::UUID(SelfSignedId)) {
        return Error(fmt::format("SelfSignedId must be parseable as a valid UUID, but got {}", DeviceID));
    }

    if (!Validate::Address(ServerAddress)) {
        return Error(fmt::format("ServerAddress must be resolveable as a UDP address, but got {}", ServerAddress));
    }

    auto err = Validate::Base64DecodeLength(SkinData, SkinImageHeight*SkinImageWidth*4);
    if (err.errored) {
        return Error(fmt::format("SkinData is invalid: {}", err.message));
    }

    err = Validate::Base64DecodeLength(CapeData, CapeImageHeight*CapeImageWidth*4);
    if (err.errored) {
        return Error(fmt::format("CapeData is invalid: {}", err.message));
    }

    int i = 0;
    for(auto &anim: AnimatedImageData) {
        err = Validate::Base64DecodeLength(anim.Image, anim.ImageHeight * anim.ImageWidth*4);
        if (err.errored) {
            return Error(fmt::format("Animated Image {} invalid data: {}", i, err.message));
            return err;
        }
        if (anim.Type < 0 || anim.Type > 3) {
            return Error(fmt::format("Invalid animation type {}", anim.Type));
        }

        i++;
    }
    try {
        auto d = Base64::Decode(SkinGeometry);
        if (!Validate::JSON(std::string(d.begin(), d.end()))) {
            return Error("SkinGeometry does not contain a valid base64 encoded json");
        }
    } catch (std::invalid_argument &e) {
        return Error(fmt::format("SkinGeometry is not a valid base64: {}", e.what()));
    }


    try {
        auto d = Base64::Decode(SkinResourcePatch);
        if (!Validate::JSON(std::string(d.begin(), d.end()))) {
            return Error("SkinResourcePatch does not contain a valid base64 encoded json");
        }
    } catch (std::invalid_argument &e) {
        return Error(fmt::format("SkinResourcePatch is not a valid base64: {}", e.what()));
    }

    if (SkinId.size() == 0) {
        return Error("SkinId must no be empty");
    }

    if (UIProfile != 0 && UIProfile != 1) {
        return Error(fmt::format("UIProfile must be either 0 or 1 but got {}", UIProfile));
    }

    return Error();
}

std::string emptySkin() {
    std::vector<uint8_t> v;
    v.resize(32*64*4);
    memset(v.data(), 0, v.size());
    for (int i = 0; i < 32*64; i++) {
        v[i*4+3] = 255; // Set alpha
    }
    return Base64::Encode(v);
}

ClientData::ClientData() {
    OS = DeviceAndroid;
    GameVersion = CurrentVersion;
    DeviceID = CppCommon::UUID::Random().string();
    LanguageCode = "en_US";
    SelfSignedId = CppCommon::UUID::Random().string();
    SkinId = CppCommon::UUID::Random().string();
    SkinData = emptySkin();
    SkinImageHeight = 32;
    SkinImageWidth = 64;
    SkinResourcePatch = Base64::Encode(Resources::DefaultSkinResourcePatch);
    SkinGeometry = Base64::Encode(Resources::DefaultSkinGeometry);
}