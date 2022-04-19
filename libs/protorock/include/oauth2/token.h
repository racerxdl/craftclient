#pragma once

#include <date/date.h>
#include <jsoncpp/json/json.h>

#include <chrono>
#include <string>
#include <fstream>
#include <fmt/format.h>

#include "common/common.h"
#include "common/json.h"

namespace ProtoRock {
namespace OAuth2 {

using namespace ProtoRock::Common;

struct Token : public JsonSerializable {
    // AccessToken is the token that authorizes and authenticates
    // the requests.
    std::string AccessToken;

    // TokenType is the type of token.
    // The Type method returns either this or "Bearer", the default.
    std::string TokenType;

    // RefreshToken is a token that's used by the application
    // (as opposed to the user) to refresh the access token
    // if it expires.
    std::string RefreshToken;

    // Expiry is the optional expiration time of the access token.
    //
    // If zero, TokenSource implementations will reuse the same
    // token forever and RefreshToken or equivalent
    // mechanisms for that TokenSource will not be used.
    Time Expiry;

    int ExpiresIn;
    std::string Scope;

    void Serialize(Json::Value &root) const override {
        root["access_token"] = AccessToken;
        if (TokenType.size() > 0) root["token_type"] = TokenType;
        if (RefreshToken.size() > 0) root["refresh_token"] = RefreshToken;
        if (Expiry.time_since_epoch().count() != 0) {
            root["expiry"] = date::format("%FT%TZ", Expiry);
        }
        root["scope"] = Scope;
        root["expires_in"] = ExpiresIn;
    }

    void Deserialize(Json::Value &root) override {
        AccessToken = root.get("access_token", "").asString();
        RefreshToken = root.get("refresh_token", "").asString();
        TokenType = root.get("token_type", "").asString();
        Scope = root.get("scope", "").asString();
        std::cout << root.get("expires_in", 0).asString() << std::endl;
        ExpiresIn = root.get("expires_in",-1).asInt();
        if (ExpiresIn != 0 && ExpiresIn != -1) {
            Expiry = Now() + std::chrono::seconds(ExpiresIn);
        }
    }

    Token() {}
    Token(const Token &b) : AccessToken(b.AccessToken), RefreshToken(b.RefreshToken), TokenType(b.TokenType), Expiry(b.Expiry) {}
    Token(const std::string &jsonString) { Common::CJsonSerializer::Deserialize(this, jsonString); }

    void Save(const std::string &filename) {
        std::ofstream f;
        f.open(filename);
        if (!f.is_open()) {
            throw Exception(fmt::format("cannot open file {}", filename));
        }
        f << CJsonSerializer::Serialize(this);
        f.close();
    }

    static Token LoadFromFile(const std::string &filename) {
        std::ifstream f;
        f.open(filename);
        if (!f.is_open()) {
            throw Exception(fmt::format("cannot open file {}", filename));
        }
        auto ss = std::ostringstream();
        ss << f.rdbuf();
        f.close();
        return Token(ss.str());
    }
};
}  // namespace OAuth2
}  // namespace ProtoRock