#pragma once

#include <list>
#include <memory>
#include <mutex>

#include "common/common.h"
#include "common/compressor.h"
#include "common/decompressor.h"

namespace ProtoRock {
namespace Common {

#include <type_traits>

template <typename T, typename F>
constexpr auto has_member_impl(F&& f) -> decltype(f(std::declval<T>()), true) {
    return true;
}

template <typename>
constexpr bool has_member_impl(...) { return false; }

#define has_member(T, EXPR) \
    has_member_impl<T>([](auto&& obj) -> decltype(obj.EXPR) {})

template <class T>
class ObjectPool {
   private:
    std::list<std::shared_ptr<T>> resources;
    std::mutex mtx;

   public:
    std::shared_ptr<T> Get() {
        std::lock_guard<std::mutex> lock(mtx);
        if (resources.empty()) {
            return std::make_shared<T>();
        }
        auto r = resources.front();
        resources.pop_front();
        if constexpr(has_member(T, resize())) {
            r->resize(0);
        } else if constexpr(has_member(T, clear())) {
            r->clear();
        }
        return r;
    }
    void Return(std::shared_ptr<T> r) {
        std::lock_guard<std::mutex> lock(mtx);
        resources.push_back(r);
    }
};
struct ObjectPools {
    static ObjectPool<PacketBuff> PacketBuffPool;
    static ObjectPool<ByteBuffer> ByteBufferPool;
    static ObjectPool<Compressor> CompressorPool;
    static ObjectPool<Decompressor> DecompressorPool;
};
}  // namespace Common
}  // namespace ProtoRock