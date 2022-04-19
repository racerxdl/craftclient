#pragma once

#include <endian.h>

#include <asio.hpp>
#include <chrono>
#include <cstdint>
#include <exception>
#include <queue>
#include <string>
#include <vector>

#include "common/common.h"

namespace ProtoRock {

class RaknetException : public Common::Exception {
   public:
    RaknetException(const std::string &msg) : Common::Exception(msg) {}
};

typedef asio::ip::udp::endpoint UDPAddress;

}  // namespace ProtoRock