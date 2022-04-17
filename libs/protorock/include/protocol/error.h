#pragma once

#include <string>

namespace ProtoRock {
struct Error {
    bool errored = false;
    std::string message;

    Error() : errored(false), message("") {}
    Error(const std::string &msg) : errored(true), message(msg) {}
};
}  // namespace ProtoRock