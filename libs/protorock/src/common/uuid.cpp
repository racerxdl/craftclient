#include "common/common.h"

std::string ProtoRock::Common::UUID::str() {
    std::string s;
    s.resize(37);
    uuid_unparse(value, s.data());
    s.resize(36);
    return s;
}