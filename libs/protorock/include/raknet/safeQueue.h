#pragma once

#include <mutex>
#include <queue>

namespace ProtoRock {
template <class T>
class SafeQueue {
   private:
    std::mutex mtx;
    std::queue<T> q;

   public:
    void Put(T v) {
        std::lock_guard<std::mutex> lock(mtx);
        q.push(v);
    }
    std::pair<T, bool> Get() {
        T v;
        std::lock_guard<std::mutex> lock(mtx);
        if (q.empty()) {
            return std::pair(v, false);
        }
        v = q.front();
        q.pop();
        return std::pair(v, true);
    }
    int Size() {
        std::lock_guard<std::mutex> lock(mtx);
        return q.size();
    }
};
}  // namespace ProtoRock