#include "raknet/datagramQueue.h"

namespace ProtoRock {
bool datagramQueue::put(uint24_t id) {
    if (id < lowest) {
        return false;
    }
    if (data.count(id) > 0) {
        return false;
    }
    if (id >= highest) {
        highest = id + 1;
    }

    data[id] = id;

    return true;
}

void datagramQueue::clear() {
    uint24_t index;

    for (index = lowest; index < highest; index++) {
        if (data.count(index) == 0) {
            break;
        }
        data.erase(index);
    }

    lowest = index;
}

std::vector<uint24_t> datagramQueue::missing() {
    std::vector<uint24_t> m;

    for (uint24_t index = lowest; index < highest; index++) {
        if (data.count(index) == 0) {
            m.push_back(index);
            data[index] = index;
        }
    }
    clear();

    return m;
}

uint24_t datagramQueue::windowSize() {
    return highest - lowest;
}

}  // namespace ProtoRock