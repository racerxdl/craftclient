#include "common/json.h"

#include <fmt/format.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

#include <stdexcept>

#include "common/common.h"
using namespace ProtoRock::Common;

std::string CJsonSerializer::Serialize(IJsonSerializable *pObj) {
    std::string o;
    if (!CJsonSerializer::Serialize(pObj, o)) {
        throw std::invalid_argument("error serializing json");
    }
    return o;
}

bool CJsonSerializer::Serialize(IJsonSerializable *pObj, std::string &output) {
    if (pObj == nullptr)
        return false;

    Json::Value serializeRoot;
    pObj->Serialize(serializeRoot);

    Json::StyledWriter writer;
    output = writer.write(serializeRoot);

    return true;
}

bool CJsonSerializer::Deserialize(IJsonSerializable *pObj, const std::string &input) {
    if (pObj == nullptr)
        return false;

    Json::Value deserializeRoot;
    Json::Reader reader;

    if (!reader.parse(input, deserializeRoot))
        return false;

    pObj->Deserialize(deserializeRoot);

    return true;
}

Json::Value ProtoRock::Common::BuildJsonValue(const picojson::value &val) {
    if (val.is<bool>()) return Json::Value(val.get<bool>());
    if (val.is<int64_t>()) return Json::Int64(val.get<int64_t>());
    if (val.is<double>()) return Json::Value(val.get<double>());
    if (val.is<std::string>()) return Json::Value(val.get<std::string>());

    if (val.is<picojson::array>()) {
        Json::Value v;
        auto a = val.get<picojson::array>();
        for (int i = 0; i < a.size(); i++) {
            v[i] = BuildJsonValue(a[i]);
        }
        return v;
    }
    if (val.is<picojson::object>()) {
        Json::Value v;
        auto obj = val.get<picojson::object>();
        for (auto &kv : obj) {
            v[kv.first] = BuildJsonValue(kv.second);
        }
    };
    throw Exception(fmt::format("unknown picojson value: {}", val.to_str()));
}

picojson::value ProtoRock::Common::BuildPicoValue(const Json::Value val) {
    if (val.isBool()) {
        return picojson::value(val.asBool());
    }
    if (val.isString()) {
        return picojson::value(val.asString());
    }
    if (val.isNumeric()) {
        return picojson::value(val.asDouble());
    }
    if (val.isArray()) {
        picojson::value::array a;
        for (int i = 0; i < val.size(); i++) {
            a.push_back(picojson::value(BuildPicoValue(val[i])));
        }
        return picojson::value(a);
    }

    // Object
    picojson::value::object picoVal;
    for (auto k : val.getMemberNames()) {
        auto v = val[k];
        picoVal[k] = BuildPicoValue(v);
    }

    return picojson::value(picoVal);
}

void ProtoRock::Common::AddClaims(jwt::builder<jwt::traits::kazuho_picojson> &builder, const Json::Value val) {
    for (auto k : val.getMemberNames()) {
        builder.set_payload_claim(k, BuildPicoValue(val[k]));
    }
}

void ProtoRock::Common::FromClaims(IJsonSerializable *pObj, jwt::decoded_jwt<jwt::traits::kazuho_picojson> &jwt) {
    Json::Value v;
    for (auto &kv: jwt.get_payload_claims()) {
        v[kv.first] = BuildJsonValue(kv.second.to_json());
    }
    pObj->Deserialize(v);
}