#pragma once

#include <jsoncpp/json/json.h>

#include <string>

#include "common/common.h"
#include "oauth2/token.h"

namespace ProtoRock {
namespace OAuth2 {

const std::string minecraftClientId = "0000000048183522";

struct DeviceAuthConnect : public JsonSerializable {
    std::string UserCode;
    std::string DeviceCode;
    std::string VerificationURI;
    int Interval;
    int ExpiresIn;

    void Serialize(Json::Value &root) const override {
        root["user_code"] = UserCode;
        root["device_code"] = DeviceCode;
        root["verification_uri"] = VerificationURI;
        root["interval"] = Interval;
        root["expires_in"] = ExpiresIn;
    }

    void Deserialize(Json::Value &root) override {
        UserCode = root.get("user_code", "").asString();
        DeviceCode = root.get("device_code", "").asString();
        VerificationURI = root.get("verification_uri", "").asString();
        Interval = root.get("interval", "").asInt();
        ExpiresIn = root.get("expires_in", "").asInt();
    }

    DeviceAuthConnect() {}
    DeviceAuthConnect(const std::string &jsonString) { CJsonSerializer::Deserialize(this, jsonString); }
};

struct DeviceAuthPoll : public Token {
    std::string Error;
    std::string UserId;

    void Serialize(Json::Value &root) const override {
        Token::Serialize(root);
        root["error"] = Error;
        root["user_id"] = UserId;
    }

    void Deserialize(Json::Value &root) override {
        Token::Deserialize(root);
        Error = root.get("error", "").asString();
        UserId = root.get("user_id", "").asString();
    }

    DeviceAuthPoll() {}
    DeviceAuthPoll(const std::string &jsonString) { CJsonSerializer::Deserialize(this, jsonString); }
};

DeviceAuthConnect StartDeviceAuth();
bool PollDeviceAuth(const std::string &deviceCode, Token &tkn);
void RefreshToken(Token &tkn);
}  // namespace OAuth2
}  // namespace ProtoRock