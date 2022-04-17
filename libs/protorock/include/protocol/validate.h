#pragma once

#include <string>

#include "protocol/error.h"

namespace ProtoRock {
namespace Validate {

bool Username(const std::string &v);
bool Version(const std::string &v);
bool UUID(const std::string &v);
bool StringToInt(const std::string &v);
bool Address(const std::string &v);
Error Base64DecodeLength(const std::string &v, int len);
bool Base64(const std::string &v);
bool JSON(const std::string &v);

}  // namespace Validation
}  // namespace ProtoRock