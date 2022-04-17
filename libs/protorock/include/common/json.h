#pragma once

#include <jsoncpp/json/json.h>
#include <jwt-cpp/jwt.h>

#include <memory>

namespace ProtoRock {
namespace Common {

struct IJsonSerializable {
    virtual ~IJsonSerializable() = default;
    virtual void Serialize(Json::Value &root) const = 0;
    virtual void Deserialize(Json::Value &root) = 0;
};

class CJsonSerializer {
   public:
    static bool Serialize(IJsonSerializable *pObj, std::string &output);
    static std::string Serialize(IJsonSerializable *pObj);
    static bool Deserialize(IJsonSerializable *pObj, const std::string &input);

   private:
    CJsonSerializer() = default;
};

struct JsonSerializable : public IJsonSerializable {
    std::string JsonString() { return CJsonSerializer::Serialize(this); }
};

void AddClaims(jwt::builder<jwt::traits::kazuho_picojson> &builder, const Json::Value val);
void FromClaims(IJsonSerializable *pObj, jwt::decoded_jwt<jwt::traits::kazuho_picojson> &jwt);
picojson::value BuildPicoValue(const Json::Value val);
Json::Value BuildJsonValue(const picojson::value &val);
}  // namespace Common
}  // namespace ProtoRock