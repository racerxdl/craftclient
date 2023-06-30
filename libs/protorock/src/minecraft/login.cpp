#include "minecraft/login.h"

#include <jsoncpp/json/json.h>
#include <jwt-cpp/jwt.h>

using namespace ProtoRock;
using namespace ProtoRock::Common;
using namespace ProtoRock::Login;
using namespace std::literals;

Common::ByteBuffer Login::Encode(const std::string &chainData, const Protocol::ClientData &clientData, const Crypto::KeyPair &keyPair) {
    Json::Value decodedChain;
    Json::Reader reader;

    if (!reader.parse(chainData, decodedChain)) {
        throw Exception("failed to parse chainData: invalid json");
    }

    auto chain = decodedChain["chain"][0].asString();
    auto chainWithUData = decodedChain["chain"][1].asString();
    auto decoded = jwt::decode(chain);
    auto x5u = decoded.get_header_claims()["x5u"];

    auto jwtKey = jwt::algorithm::es384(keyPair.PublicToPEM(), keyPair.PrivateToPEM());
    auto token = jwt::create()
                     .set_type("JWS")
                     .set_expires_at(Now() + 6h)
                     .set_not_before(Now() - 6h)
                     .set_header_claim("x5u", jwt::claim(Base64::Encode(keyPair.PKIXPublicKey())))
                     .set_payload_claim("identityPublicKey", x5u)
                     .set_payload_claim("certificateAuthority", jwt::claim(picojson::value(true)))
                     .sign(jwtKey);

    // The token needs to be first in the chain, so we shift everything
    decodedChain["chain"][2] = decodedChain["chain"][1];
    decodedChain["chain"][1] = decodedChain["chain"][0];
    decodedChain["chain"][0] = token;

    auto jwts = jwt::create();
    clientData.AddClaims(jwts);
    auto rawToken = jwts.set_header_claim("x5u", jwt::claim(Base64::Encode(keyPair.PKIXPublicKey()))).sign(jwtKey);
    decodedChain["RawToken"] = rawToken;
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    auto data = Json::writeString(builder, decodedChain);
    PacketBuff b;

    b.WriteLE((int32_t)(data.size()));
    b.Write(data);

    b.WriteLE((int32_t)(rawToken.size()));
    b.Write(rawToken);

    return b.ToBuffer();
}

Common::ByteBuffer Login::EncodeOffline(const Protocol::IdentityData &identityData, const Protocol::ClientData &clientData, const Crypto::KeyPair &keyPair) {
    Json::Value extraDataJson;

    identityData.Serialize(extraDataJson);

    auto keyData = Base64::Encode(keyPair.PKIXPublicKey());
    auto extraData = Common::BuildPicoValue(extraDataJson);
    auto jwtKey = jwt::algorithm::es384(keyPair.PublicToPEM(), keyPair.PrivateToPEM());
    auto token = jwt::create()
                     .set_type("JWS")
                     .set_expires_at(Now() + 6h)
                     .set_not_before(Now() - 6h)
                     .set_header_claim("x5u", jwt::claim(keyData))
                     .set_payload_claim("identityPublicKey", jwt::claim(keyData))
                     .set_payload_claim("extraData", jwt::claim(extraData))
                     .sign(jwtKey);

    Json::Value chain;
    chain["chain"].append(token);

    auto jwts = jwt::create();
    clientData.AddClaims(jwts);
    auto rawToken = jwts.set_header_claim("x5u", jwt::claim(keyData)).sign(jwtKey);
    chain["RawToken"] = rawToken;
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    auto data = Json::writeString(builder, chain);
    PacketBuff b;

    b.WriteLE((int32_t)(data.size()));
    b.Write(data);

    b.WriteLE((int32_t)(rawToken.size()));
    b.Write(rawToken);

    return b.ToBuffer();
}

ParsedLogin Login::Parse(const ByteBuffer &request) {
    auto b = PacketBuff(request);
    auto chainLength = b.ReadU32LE();
    auto chainBytes = b.ReadBytes(chainLength);
    auto chainData = std::string(chainBytes.begin(), chainBytes.end());
    Json::Value chainRoot;
    Json::Reader reader;

    if (!reader.parse(chainData, chainRoot)) {
        throw Exception("cannot parse login: invalid chain json");
    }
    auto chain = chainRoot["chain"];
    if (chain.size() < 1) {
        throw Exception("JWT chain should have at least one token");
    }
    auto rawTokenLength = b.ReadU32LE();
    auto rawTokenBytes = b.ReadBytes(rawTokenLength);
    auto rawToken = std::string(rawTokenBytes.begin(), rawTokenBytes.end());

    auto decoded = jwt::decode(chain[0].asString());
    auto key = decoded.get_header_claims()["x5u"].as_string();
    auto authenticated = false;
    auto identityData = Protocol::IdentityData();

    if (chain.size() == 1) {
        // Player was not authenticated with XBOX Live, meaning the one token in here is self-signed.
        auto publicKey = decoded.get_payload_claim("identityPublicKey");
        auto extraDataClaim = decoded.get_payload_claim("extraData");
        try {
            jwt::verify()
                .allow_algorithm(jwt::algorithm::es384{key})
                .verify(decoded);
        } catch (jwt::signature_verification_exception &e) {
            throw Exception(fmt::format("error validating signature: {}", e.what()));
        }
        if (!CJsonSerializer::Deserialize(&identityData, extraDataClaim.as_string())) {
            throw Exception("invalid json encountered when parsing extraData to identityData");
        }
    } else if (chain.size() == 3) {
        // Player was (or should be) authenticated with XBOX Live, meaning the chain is exactly 3 tokens
        // long.
        auto mojangKey = Crypto::KeyPair(MojangPublicKey);
        auto identityKey = Crypto::KeyPair(Base64::Decode(decoded.get_payload_claim("identityPublicKey").as_string()));
        auto jwt0Key = Crypto::KeyPair(Base64::Decode(key));

        auto algo = jwt::algorithm::es384{jwt0Key.PublicToPEM()};
        auto verifier0 = jwt::verify().allow_algorithm(algo);
        try {
            verifier0.verify(decoded);
        } catch (jwt::signature_verification_exception &e) {
            throw Exception(fmt::format("error validating signature chain0: {}", e.what()));
        }
        authenticated = mojangKey.X() == identityKey.X() && mojangKey.Y() == identityKey.Y();
        auto chain1 = jwt::decode(chain[1].asString());
        auto chain2 = jwt::decode(chain[2].asString());
        auto key1 = chain1.get_header_claims()["x5u"].as_string();
        auto key2 = chain2.get_header_claims()["x5u"].as_string();
        auto jwt1Key = Crypto::KeyPair(Base64::Decode(key1));
        auto jwt2Key = Crypto::KeyPair(Base64::Decode(key2));

        try {
            jwt::verify().allow_algorithm(jwt::algorithm::es384{jwt1Key.PublicToPEM()}).verify(chain1);
        } catch (jwt::signature_verification_exception &e) {
            throw Exception(fmt::format("error validating signature for chain1: {}", e.what()));
        }
        try {
            jwt::verify().allow_algorithm(jwt::algorithm::es384{jwt2Key.PublicToPEM()}).verify(chain2);
        } catch (jwt::signature_verification_exception &e) {
            throw Exception(fmt::format("error validating signature for chain2: {}", e.what()));
        }
        auto extraDataClaim = chain2.get_payload_claim("extraData").to_json().serialize();
        if (!CJsonSerializer::Deserialize(&identityData, extraDataClaim)) {
            throw Exception("invalid json encountered when parsing extraData to identityData");
        }

        if (!authenticated) {
            // Validate identity
            if (!identityData.XUID.empty()) {
                throw Exception("Identity data should only have XUID when logged in to XBOX Live");
            }
            if (!identityData.TitleId.empty()) {
                throw Exception("Identity data should only have TitleID when logged in to XBOX Live");
            }
        }
    } else {
        throw Exception(fmt::format("unexpected login chain length {}", chain.size()));
    }

    auto decodedRaw = jwt::decode(rawToken);
    Protocol::ClientData clientData;
    clientData.FromClaims(decodedRaw);
    clientData.Validate();
    return ParsedLogin{
        identityData,
        clientData,
        Protocol::AuthResult{
            Base64::Decode(key),
            authenticated,
        }};
}