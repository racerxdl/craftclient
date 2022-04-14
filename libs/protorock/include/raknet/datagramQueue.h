#pragma once

#include <unordered_map>

#include "common.h"

namespace ProtoRock {
struct datagramQueue {
    uint24_t lowest = 0, highest = 0;
    std::unordered_map<uint24_t, uint24_t> data;

    bool put(uint24_t);
    void clear();
    std::vector<uint24_t> missing();
    uint24_t windowSize();
};
}  // namespace ProtoRock