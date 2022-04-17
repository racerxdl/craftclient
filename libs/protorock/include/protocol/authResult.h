#pragma once

#include "common/common.h"

namespace ProtoRock {
namespace Protocol {
struct AuthResult {
    Common::ByteBuffer publicKey;
    bool XBOXLiveAuthenticated;
};
}  // namespace Protocol
}  // namespace ProtoRock