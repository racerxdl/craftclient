#pragma once

#include <fmt/format.h>

#include <cstdint>
#include <list>
#include <unordered_map>

#include "common/common.h"
#include "protocol/packet/id.h"
#include "protocol/packet/packet.h"

namespace ProtoRock {
namespace Protocol {
namespace Packet {

class PacketPool {
   private:
    std::unordered_map<PacketID, PacketList> pool;
    std::unordered_map<PacketID, PacketCreateFunction> constructors;
    std::mutex mtx;

    IPacket NewPacket(PacketID id) {
        if (!constructors.count(id)) {
            throw Common::Exception(fmt::format("no packet constructor registered for id {}", id));
        }
        return constructors[id]();
    }

   public:
    PacketPool();
    IPacket Get(PacketID id) {
        std::lock_guard<std::mutex> lock(mtx);
        if (!pool.count(id) || pool[id].size() == 0) {  // No pool
            return NewPacket(id);
        }

        auto r = pool[id].front();
        pool[id].pop_front();
        return r;
    }

    void Return(IPacket r) {
        std::lock_guard<std::mutex> lock(mtx);
        if (!pool.count(r->Id())) {
            pool[r->Id()] = std::list<IPacket>();
        }
        pool[r->Id()].push_back(r);
    }

    void Register(PacketID id, PacketCreateFunction constructor) {
        std::lock_guard<std::mutex> lock(mtx);
        pool[id] = std::list<IPacket>();
        constructors[id] = constructor;
    }
    static PacketPool Pool;
};
// IPacket
}  // namespace Packet
}  // namespace Protocol
}  // namespace ProtoRock
