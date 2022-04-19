#include "common/common.h"

std::string ProtoRock::Common::UUID::str() {
    std::string s;
    s.resize(37);
    uuid_unparse(value, s.data());
    return s;
}