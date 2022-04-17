#pragma once

#include <string>

namespace ProtoRock {
namespace Protocol {
	// CurrentProtocol is the current protocol version for the version below.
	const int CurrentProtocol = 486;
	// CurrentVersion is the current version of Minecraft as supported by the `packet` package.
	const std::string CurrentVersion = "1.18.10";
}
}  // namespace ProtoRock