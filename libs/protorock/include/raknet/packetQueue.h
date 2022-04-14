#pragma once

#include <unordered_map>
#include "common.h"

namespace ProtoRock {
    class packetQueue {
        private:
            uint24_t lowest = 0, highest = 0;
            std::unordered_map<uint24_t, ByteBuffer> data;
        public:
            bool put(uint24_t, ByteBuffer);
            std::vector<ByteBuffer> fetch();
            uint24_t windowSize();
    };
}