#pragma once

#include <jsoncpp/json/value.h>

#include <memory>

namespace ProtoRock {
namespace Common {

struct IJsonSerializable {
    virtual ~IJsonSerializable() = default;
    virtual void Serialize(Json::Value &root) = 0;
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

}  // namespace Common
}  // namespace ProtoRock