#pragma once

#include <asio.hpp>

#include <atomic>
#include <cstdint>
#include <exception>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "common.h"
#include "state.h"
#include "udpClient.h"

namespace ProtoRock {
using asio::ip::udp;
inline Time defaultDeadline() {
    using namespace std::literals;
    return std::chrono::system_clock::now() + 60s;
}

class RaknetClient : public std::enable_shared_from_this<RaknetClient> {
   private:
   protected:
    ByteBuffer buffer;
    std::atomic<bool> stop = false;
    std::atomic<bool> connected = false;
    std::atomic<bool> serviceStarted = false;
    std::mutex connMtx;
    State connection;
    int mtuSize;
    std::thread ioThread;
    std::shared_ptr<UDPClient> client;
    std::shared_ptr<asio::io_service> ioService;

    void ioRoutine();

    void onConnected();
    void onReceived(const asio::ip::udp::endpoint& endpoint, const ByteBuffer &b);
    void onError(int error, const std::string& category, const std::string& message);

    std::shared_ptr<RaknetClient> getRaknetClient() { return shared_from_this(); }
    std::shared_ptr<asio::io_service> getService() { return ioService; }
   public:
    RaknetClient(int16_t mtuSize);
    void Connect(const std::string &address, int port);
    void Start();
    void DisconnectAndStop();
    int Write(const ByteBuffer&);
    SharedByteBuffer Read();
    bool IsConnected();

    ByteBuffer Ping(const std::string &address, int port, Time deadline = defaultDeadline());

    friend class state;
};

}  // namespace ProtoRock