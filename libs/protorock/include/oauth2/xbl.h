#pragma once

#include <fmt/format.h>
#include <jsoncpp/json/json.h>

#include <fstream>
#include <string>

#include "common/common.h"
#include "oauth2/token.h"
#include "common/crypto.h"

namespace ProtoRock {
namespace OAuth2 {
struct UserInfo : public JsonSerializable {
    std::string GamerTag;
    std::string XUID;
    std::string UserHash;

    void Serialize(Json::Value &root) const override {
        root["gtg"] = GamerTag;
        root["xid"] = XUID;
        root["uhs"] = UserHash;
    }
    void Deserialize(Json::Value &root) override {
        GamerTag = root.get("gtg", "").asString();
        XUID = root.get("xid", "").asString();
        UserHash = root.get("uhs", "").asString();
    }

    UserInfo() {}
    UserInfo(const std::string &jsonString) { CJsonSerializer::Deserialize(this, jsonString); }
    UserInfo(Json::Value &json) { Deserialize(json); }
};
struct DisplayClaims : public JsonSerializable {
    std::vector<UserInfo> userInfo;
    void Serialize(Json::Value &root) const override {
        int i = 0;
        for(auto &ui: userInfo) {
            ui.Serialize(root["xui"][i]);
            i++;
        }
    }
    void Deserialize(Json::Value &root) override {
        auto xui = root["xui"];
        for (int i = 0; i < xui.size(); i++) {
            userInfo.push_back(UserInfo(xui[i]));
        }
    }

    DisplayClaims() {}
    DisplayClaims(const std::string &jsonString) { CJsonSerializer::Deserialize(this, jsonString); }
    DisplayClaims(Json::Value &json) { Deserialize(json); }
};

struct SubToken : public JsonSerializable {
    DisplayClaims claims;
    std::string Token;
    std::string IssueInstant;
    std::string NotAfter;

    void Serialize(Json::Value &root) const override {
        claims.Serialize(root["DisplayClaims"]);
        root["IssueInstant"] = IssueInstant;
        root["NotAfter"] = NotAfter;
        root["Token"] = Token;
    }
    void Deserialize(Json::Value &root) override {
        claims.Deserialize(root["DisplayClaims"]);
        IssueInstant = root.get("IssueInstant", "").asString();
        NotAfter = root.get("NotAfter", "").asString();
        Token = root.get("Token", "").asString();
    }
    SubToken() {}
    SubToken(const std::string &jsonString) { CJsonSerializer::Deserialize(this, jsonString); }
    SubToken(Json::Value &json) { Deserialize(json); }
};

struct XBLToken : public JsonSerializable {
    std::string DeviceToken;
    std::string WebPage;
    std::string Sandbox;
    bool UseModernGamertag;
    SubToken authorizationToken;
    SubToken titleToken;
    SubToken userToken;

    void Serialize(Json::Value &root) const override {
        authorizationToken.Serialize(root["AuthorizationToken"]);
        titleToken.Serialize(root["TitleToken"]);
        userToken.Serialize(root["UserToken"]);
        root["DeviceToken"] = DeviceToken;
        root["WebPage"] = WebPage;
        root["Sandbox"] = Sandbox;
        root["UseModernGamertag"] = UseModernGamertag;
    }
    void Deserialize(Json::Value &root) override {
        authorizationToken.Deserialize(root["AuthorizationToken"]);
        titleToken.Deserialize(root["TitleToken"]);
        userToken.Deserialize(root["UserToken"]);
        DeviceToken = root.get("DeviceToken", "").asString();
        WebPage = root.get("WebPage", "").asString();
        Sandbox = root.get("Sandbox", "").asString();
        UseModernGamertag = root.get("UseModernGamertag", false).asBool();
    }
    XBLToken() {}
    XBLToken(const std::string &jsonString) { CJsonSerializer::Deserialize(this, jsonString); }
    void Save(const std::string &filename) {
        std::ofstream f;
        f.open(filename);
        if (!f.is_open()) {
            throw Exception(fmt::format("cannot open file {}", filename));
        }
        f << CJsonSerializer::Serialize(this);
        f.close();
    }

    static XBLToken LoadFromFile(const std::string &filename) {
        std::ifstream f;
        f.open(filename);
        if (!f.is_open()) {
            throw Exception(fmt::format("cannot open file {}", filename));
        }
        auto ss = std::ostringstream();
        ss << f.rdbuf();
        f.close();
        return XBLToken(ss.str());
    }
};

XBLToken RequestXBLToken(const Token &liveToken, const std::string &relyingParty);
std::string RequestMinecraftChain(const Crypto::KeyPair &keyPair, const XBLToken &tkn);
}  // namespace OAuth2
}  // namespace ProtoRock