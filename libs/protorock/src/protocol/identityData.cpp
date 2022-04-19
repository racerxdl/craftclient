#include "protocol/identityData.h"

#include <fmt/format.h>

#include "common/common.h"
#include "protocol/validate.h"

using namespace ProtoRock;
using namespace ProtoRock::Protocol;

IdentityData::IdentityData() {
    Identity = Common::UUID::NewRandom().str();
    DisplayName = "Steve";
}

Error IdentityData::Validate() {
    if (XUID.size() != 0 && !Validate::StringToInt(XUID)) {
        return Error(fmt::format("XUID must be parseable as an int64_t, but got {}", XUID));
    }

    if (!Validate::UUID(Identity)) {
        return Error(fmt::format("UUID must be parseable as a valid UUID, but got {}", Identity));
    }

    if (DisplayName.size() == 0 || DisplayName.size() > 15) {
        return Error(fmt::format("DisplayName must not be empty or longer than 15 characters, but got {} characters", DisplayName.size()));
    }

    if (DisplayName[0] >= '0' && DisplayName[0] <= '9') {
        return Error(fmt::format("DisplayName may not have a number as first character, but got {}", DisplayName));
    }

    if (DisplayName[0] == ' ' || DisplayName[DisplayName.size() - 1] == ' ') {
        return Error(fmt::format("DisplayName may not have a space as first/last character, but got {}", DisplayName));
    }

    if (!Validate::Username(DisplayName)) {
        return Error(fmt::format("DisplayName must only contain numbers, letters and spaces, but got {}", DisplayName));
    }

    // We check here if the name contains at least 2 spaces after each other, which is not allowed. The name
    // is only allowed to have single spaces.
    if (DisplayName.find("  ") != std::string::npos) {
        return Error(fmt::format("DisplayName must only have single spaces, but got {}", DisplayName));
    }

    return Error();
}