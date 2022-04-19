#include "protocol/clientData.h"

#include <regex>
#include <stdexcept>

#include <fmt/core.h>
#include <asio.hpp>
#include <jsoncpp/json/json.h>

#include "protocol.h"
#include "protocol/validate.h"
#include "common/base64.h"
#include "common/common.h"
#include "resources/skin.h"

using namespace ProtoRock;
using namespace ProtoRock::Protocol;


Error ClientData::Validate() const {
    if (OS <= 0 || OS > 13) {
        return Error(fmt::format("DeviceOS must carry a value between 1 and 13, but got {}", OS));
    }

    if (!Validate::UUID(DeviceId)) {
        return Error(fmt::format("DeviceID must be parseable as a valid UUID, but go {}", DeviceId));
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
        return Error(fmt::format("SelfSignedId must be parseable as a valid UUID, but got {}", DeviceId));
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
    DeviceId = Common::UUID::NewRandom().str();
    LanguageCode = "en_US";
    SelfSignedId = Common::UUID::NewRandom().str();
    SkinId = Common::UUID::NewRandom().str();
    SkinData = emptySkin();
    SkinImageHeight = 32;
    SkinImageWidth = 64;
    SkinResourcePatch = Base64::Encode(Resources::DefaultSkinResourcePatch);
    SkinGeometry = Base64::Encode(Resources::DefaultSkinGeometry);
}

void ClientData::Serialize(Json::Value &root) const {
    for(const auto &a: AnimatedImageData) {
        auto v = Json::Value();
        a.Serialize(v);
        root["AnimatedImageData"].append(v);
    }
    root["CapeData"] = CapeData;
    root["CapeId"] = CapeId;
    root["CapeImageHeight"] = CapeImageHeight;
    root["CapeImageWidth"] = CapeImageWidth;
    root["CapeOnClassicSkin"] = CapeOnClassicSkin;
    root["ClientRandomId"] = Json::Value::Int64(ClientRandomId);
    root["CurrentInputMode"] = CurrentInputMode;
    root["DefaultInputMode"] = DefaultInputMode;
    root["DeviceModel"] = DeviceModel;
    root["DeviceOS"] = OS;
    root["DeviceId"] = DeviceId;
    root["GameVersion"] = GameVersion;
    root["GuiScale"] = GuiScale;
    root["LanguageCode"] = LanguageCode;
    root["PersonaSkin"] = PersonaSkin;
    root["PlatformOfflineId"] = PlatformOfflineId;
    root["PlatformOnlineId"] = PlatformOnlineId;
    root["PlatformUserId"] = PlatformUserId;
    root["PremiumSkin"] = PremiumSkin;
    root["SelfSignedId"] = SelfSignedId;
    root["ServerAddress"] = ServerAddress;
    root["SkinAnimationData"] = SkinAnimationData;
    root["SkinData"] = SkinData;
    root["SkinGeometryData"] = SkinGeometry;
    root["SkinGeometryDataEngineVersion"] = SkinGeometryVersion;
    root["SkinId"] = SkinId;
    root["PlayFabId"] = PlayFabId;
    root["SkinImageHeight"] = SkinImageHeight;
    root["SkinImageWidth"] = SkinImageWidth;
    root["SkinResourcePatch"] = SkinResourcePatch;
    root["SkinColor"] = SkinColor;
    root["ArmSize"] = ArmSize;
    for (int i = 0; i < PersonaPieces.size(); i++) {
        auto v = Json::Value();
        PersonaPieces[i].Serialize(v);
        root["PersonaPieces"].append(v);
    }
    for (int i = 0; i < PieceTintColors.size(); i++) {
        auto v = Json::Value();
        PieceTintColors[i].Serialize(v);
        root["PieceTintColors"].append(v);
    }
    root["ThirdPartyName"] = ThirdPartyName;
    root["ThirdPartyNameOnly"] = ThirdPartyNameOnly;
    root["UIProfile"] = UIProfile;
}

void ClientData::Deserialize(Json::Value &root) {
    for (int i = 0; i < root["AnimatedImageData"].size(); i++) {
        SkinAnimation sa;
        sa.Deserialize(root["AnimatedImageData"][i]);
        AnimatedImageData.push_back(sa);
    }
    CapeData = root.get("CapeData", "").asString();
    CapeId = root.get("CapeId", "").asString();
    CapeImageWidth = root.get("CapeImageWidth", 0).asInt();
    CapeImageHeight = root.get("CapeImageHeight", 0).asInt();
    CapeOnClassicSkin = root.get("CapeOnClassicSkin", false).asBool();
    ClientRandomId = root.get("ClientRandomId", 0).asInt64();
    CurrentInputMode = root.get("CurrentInputMode", 0).asInt();
    DefaultInputMode = root.get("DefaultInputMode", 0).asInt();
    DeviceModel = root.get("DeviceModel", "").asString();
    OS = (DeviceOS) (root.get("DeviceOS", DeviceAndroid).asInt());
    DeviceId = root.get("DeviceId", "").asString();
    GameVersion = root.get("GameVersion", "").asString();
    GuiScale = root.get("GuiScale", 0).asInt();
    LanguageCode = root.get("LanguageCode", "").asString();
    PersonaSkin = root.get("PersonaSkin", false).asBool();
    PlatformOfflineId = root.get("PlatformOfflineId", "").asString();
    PlatformOnlineId = root.get("PlatformOnlineId", "").asString();
    PlatformUserId = root.get("PlatformUserId", "").asString();
    PremiumSkin = root.get("PremiumSkin", false).asBool();
    SelfSignedId = root.get("SelfSignedId", "").asString();
    ServerAddress = root.get("ServerAddress", "").asString();
    SkinAnimationData = root.get("SkinAnimationData", "").asString();
    SkinData = root.get("SkinData", "").asString();
    SkinGeometry = root.get("SkinGeometryData", "").asString();
    SkinGeometryVersion = root.get("SkinGeometryDataEngineVersion", "").asString();
    SkinId = root.get("SkinId", "").asString();
    PlayFabId = root.get("PlayFabId", "").asString();
    SkinImageHeight = root.get("SkinImageHeight", 0).asInt();
    SkinImageWidth = root.get("SkinImageWidth", 0).asInt();
    SkinResourcePatch = root.get("SkinResourcePatch", "").asString();
    SkinColor = root.get("SkinColor", "").asString();
    ArmSize = root.get("ArmSize", "").asString();
    for (int i = 0; i < root["PersonaPiece"].size(); i++) {
        PersonaPiece p;
        p.Deserialize(root["PersonaPiece"][i]);
        PersonaPieces.push_back(p);
    }
    for (int i = 0; i < root["PieceTintColors"].size(); i++) {
        PersonaPieceTintColor p;
        p.Deserialize(root["PieceTintColors"][i]);
        PieceTintColors.push_back(p);
    }
    ThirdPartyName = root.get("ThirdPartyName", "").asString();
    ThirdPartyNameOnly = root.get("ThirdPartyNameOnly", false).asBool();
    UIProfile = root.get("UIProfile", 0).asInt();
}

void ClientData::AddClaims(jwt::builder<jwt::traits::kazuho_picojson> &builder) const {
    auto jsonThis = Json::Value();
    Serialize(jsonThis);
    ProtoRock::Common::AddClaims(builder, jsonThis);
}

void ClientData::FromClaims(jwt::decoded_jwt<jwt::traits::kazuho_picojson> &jwt) {
    ProtoRock::Common::FromClaims(this, jwt);
}