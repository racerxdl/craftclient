#pragma once

#include <list>
#include <memory>
#include <mutex>

namespace ProtoRock {
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
            return r;
        }
        void Return(std::shared_ptr<T> r) {
            std::lock_guard<std::mutex> lock(mtx);
            resources.push_back(r);
        }
};
}  // namespace ProtoRock