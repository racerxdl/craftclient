#pragma once

#include <unordered_map>

#include "common.h"
#include "packet.h"

namespace ProtoRock {
struct recoveryQueue {
    std::unordered_map<uint24_t, Packet> queue;
    std::unordered_map<uint24_t, Time> timestamps;
    int ptr = 0;
    std::vector<TimeDuration> delays;

    void put(uint24_t index, Packet value);
    Packet take(uint24_t index);
    Packet takeWithoutDelayAdd(uint24_t index);
    Time Timestamp(uint24_t sequenceNumber) { return timestamps[sequenceNumber]; }
    TimeDuration AvgDelay();

    recoveryQueue();
};
}  // namespace ProtoRock