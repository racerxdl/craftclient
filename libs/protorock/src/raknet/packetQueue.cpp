#include "raknet/packetQueue.h"

namespace ProtoRock {
bool packetQueue::put(uint24_t index, ByteBuffer b) {
    if (index < lowest) {
        return false;
    }
    if (data.count(index) > 0) {
        return false;
    }
    if (index >= highest) {
        highest = index + 1;
    }
    data[index] = b;

    return true;
}

std::vector<ByteBuffer> packetQueue::fetch() {
    std::vector<ByteBuffer> packets;

    auto index = lowest;
    while (index < highest) {
        if (data.count(index) == 0) {
            break;
        }
        auto packet = data[index];
        data.erase(index);
        packets.push_back(packet);
        index++;
    }
    lowest = index;
    return packets;
}

uint24_t packetQueue::windowSize() {
    return highest - lowest;
}
}  // namespace ProtoRock