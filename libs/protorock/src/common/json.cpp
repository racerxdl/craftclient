#include "common/json.h"

#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include <stdexcept>

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