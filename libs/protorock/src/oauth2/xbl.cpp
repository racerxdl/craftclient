#include "oauth2/xbl.h"

#include <cpr/cpr.h>
#include <jwt-cpp/jwt.h>
#include <system/uuid.h>

#include "common/base64.h"
#include "common/crypto.h"
#include "oauth2/live.h"
#include "protocol/protocol.h"

using namespace ProtoRock::Crypto;
using namespace ProtoRock::OAuth2;

int64_t windowsTimestamp() {
    auto epoch = std::chrono::duration_cast<std::chrono::seconds>(Now().time_since_epoch()).count();
    return (epoch + 11644473600) * 10000000;
}

std::string ObtainDeviceToken(const KeyPair &keyPair) {
    auto payload = Json::Value();
    payload["RelyingParty"] = "http://auth.xboxlive.com";
    payload["TokenType"] = "JWT";
    auto properties = Json::Value();
    properties["AuthMethod"] = "ProofOfPossession";
    properties["Id"] = "{" + CppCommon::UUID::Random().string() + "}";
    properties["DeviceType"] = "Android";
    properties["Version"] = "10";
    auto proofKey = Json::Value();
    proofKey["crv"] = "P-256";
    proofKey["alg"] = "ES256";
    proofKey["use"] = "sig";
    proofKey["kty"] = "EC";
    proofKey["x"] = keyPair.X();
    proofKey["y"] = keyPair.Y();
    properties["ProofKey"] = proofKey;
    payload["Properties"] = properties;

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";  // If you want whitespace-less output

    auto data = Json::writeString(builder, payload);  // payload.toStyledString();
    auto signData = PacketBuff();
    // Signature Policy Version Bytes (0,0,0,1) + null
    auto timestamp = windowsTimestamp();
    signData.Write(0);
    signData.Write(0);
    signData.Write(0);
    signData.Write(1);
    signData.Write(0);
    signData.WriteBE(timestamp);
    signData.Write(0);

    signData.Write("POST");
    signData.Write(0);
    signData.Write("/device/authenticate");
    signData.Write(0);
    //
    signData.Write(0);
    signData.Write(data);
    signData.Write(0);

    // Build signature
    auto signatureBytes = keyPair.Sign(signData.ToBuffer());
    signData.clear();
    signData.Write(0);
    signData.Write(0);
    signData.Write(0);
    signData.Write(1);
    signData.WriteBE(timestamp);
    signData.Write(signatureBytes);
    auto sig = ProtoRock::Base64::Encode(signData.ToBuffer());

    auto headers = cpr::Header{
        {"x-xbl-contract-version", "1"},
        {"Signature", sig}};

    auto r = cpr::Post(cpr::Url{"https://device.auth.xboxlive.com/device/authenticate"},
                       headers,
                       cpr::Body{data});
    if (r.error) {
        throw Exception(fmt::format("POST https://device.auth.xboxlive.com/device/authenticate: {}", r.error.message));
    }
    if (r.status_code != 200) {
        throw Exception(fmt::format("POST https://device.auth.xboxlive.com/device/authenticate: {}", r.status_line));
    }

    Json::Value root;
    Json::Reader reader;

    if (!reader.parse(r.text, root)) {
        throw Exception("POST https://device.auth.xboxlive.com/device/authenticate: invalid json received");
    }

    return root["Token"].asString();
}

XBLToken obtainXBLToken(const KeyPair &keyPair, const Token &liveToken, const std::string &deviceToken, const std::string &relyingParty) {
    auto payload = Json::Value();
    payload["AccessToken"] = "t=" + liveToken.AccessToken;
    payload["AppId"] = minecraftClientId;
    payload["deviceToken"] = deviceToken;
    payload["Sandbox"] = "RETAIL";
    payload["UseModernGamertag"] = true;
    payload["SiteName"] = "user.auth.xboxlive.com";
    payload["RelyingParty"] = relyingParty;
    auto proofKey = Json::Value();
    proofKey["crv"] = "P-256";
    proofKey["alg"] = "ES256";
    proofKey["use"] = "sig";
    proofKey["kty"] = "EC";
    proofKey["x"] = keyPair.X();
    proofKey["y"] = keyPair.Y();
    payload["ProofKey"] = proofKey;

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";  // If you want whitespace-less output
    auto data = Json::writeString(builder, payload);

    auto signData = PacketBuff();
    // Signature Policy Version Bytes (0,0,0,1) + null
    auto timestamp = windowsTimestamp();
    signData.Write(0);
    signData.Write(0);
    signData.Write(0);
    signData.Write(1);
    signData.Write(0);
    signData.WriteBE(timestamp);
    signData.Write(0);

    signData.Write("POST");
    signData.Write(0);
    signData.Write("/authorize");
    signData.Write(0);
    //
    signData.Write(0);
    signData.Write(data);
    signData.Write(0);

    // Build signature
    auto signatureBytes = keyPair.Sign(signData.ToBuffer());
    signData.clear();
    signData.Write(0);
    signData.Write(0);
    signData.Write(0);
    signData.Write(1);
    signData.WriteBE(timestamp);
    signData.Write(signatureBytes);
    auto sig = ProtoRock::Base64::Encode(signData.ToBuffer());

    auto headers = cpr::Header{
        {"x-xbl-contract-version", "1"},
        {"Signature", sig}};

    auto r = cpr::Post(cpr::Url{"https://sisu.xboxlive.com/authorize"},
                       headers,
                       cpr::Body{data});
    if (r.error) {
        throw Exception(fmt::format("POST https://sisu.xboxlive.com/authorize: {}", r.error.message));
    }
    if (r.status_code != 200) {
        throw Exception(fmt::format("POST https://sisu.xboxlive.com/authorize: {}", r.status_line));
    }

    return XBLToken(r.text);
}

XBLToken ProtoRock::OAuth2::RequestXBLToken(const KeyPair &keyPair, const Token &liveToken, const std::string &relyingParty) {
    auto deviceToken = ObtainDeviceToken(keyPair);
    return obtainXBLToken(keyPair, liveToken, deviceToken, relyingParty);
}

std::string ProtoRock::OAuth2::RequestMinecraftChain(const KeyPair &keyPair, const XBLToken &tkn) {
    auto keyData = keyPair.PKIXPublicKey();
    auto payload = Json::Value();
    payload["identityPublicKey"] = Base64::Encode(keyData);
    auto headers = cpr::Header{
        {"Content-Type", "application/json"},
        {"Authorization", fmt::format("XBL3.0 x={};{}", tkn.authorizationToken.claims.userInfo[0].UserHash, tkn.authorizationToken.Token)},
        {"User-Agent", "MCPE/Android"},
        {"Client-Version", Protocol::CurrentVersion}};

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";  // If you want whitespace-less output

    auto data = Json::writeString(builder, payload);  // payload.toStyledString();

    auto r = cpr::Post(cpr::Url{"https://multiplayer.minecraft.net/authentication"},
                       headers,
                       cpr::Body{data});
    if (r.error) {
        throw Exception(fmt::format("POST https://multiplayer.minecraft.net/authentication: {}", r.error.message));
    }
    if (r.status_code != 200) {
        throw Exception(fmt::format("POST https://multiplayer.minecraft.net/authentication: {}", r.status_line));
    }

    return r.text;
}