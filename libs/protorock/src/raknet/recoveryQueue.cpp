#include "raknet/recoveryQueue.h"

namespace ProtoRock {

constexpr int delayRecordCount = 40;

recoveryQueue::recoveryQueue() {
    delays.resize(delayRecordCount);
}

void recoveryQueue::put(uint24_t index, Packet value) {
    queue[index] = value;
    timestamps[index] = Now();
}

Packet recoveryQueue::take(uint24_t index) {
    if (queue.count(index) > 0) {
        auto p = queue[index];
        auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(Now() - timestamps[index]);
        delays[ptr] = delay;
        ptr++;
        if (ptr == delayRecordCount) {
            ptr = 0;
        }
        queue.erase(index);
        timestamps.erase(index);
        return p;
    }
    return nullptr;
}

Packet recoveryQueue::takeWithoutDelayAdd(uint24_t index) {
    if (queue.count(index) > 0) {
        auto p = queue[index];
        queue.erase(index);
        timestamps.erase(index);
        return p;
    }
    return nullptr;
}

TimeDuration recoveryQueue::AvgDelay() {
    using namespace std::literals;
    TimeDuration average, records;

    for (auto const &delay: delays) {
        if (delay.count() == 0) {
            break;
        }
        average += delay;
        records++;
    }

    if (records.count() == 0) {
        return 50ms;
    }

    return static_cast<TimeDuration>(average.count() / records.count());
}

}  // namespace ProtoRock