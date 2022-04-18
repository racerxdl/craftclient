#pragma once

#include <server/asio/timer.h>
#include <server/asio/udp_client.h>

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

inline Time defaultDeadline() {
    using namespace std::literals;
    return std::chrono::system_clock::now() + 60s;
}

class RaknetClient : public std::enable_shared_from_this<RaknetClient> {
   private:
   protected:
    std::atomic<bool> stop;
    std::mutex connMtx;
    State connection;
    std::shared_ptr<UDPClient> client;
    std::shared_ptr<CppServer::Asio::Service> service;

    void onConnected();
    void onDisconnected();
    void onReceived(const asio::ip::udp::endpoint& endpoint, const void* buffer, size_t size);
    void onError(int error, const std::string& category, const std::string& message);
    std::shared_ptr<RaknetClient> getRaknetClient() { return shared_from_this(); }

   public:
    RaknetClient(std::shared_ptr<CppServer::Asio::Service> service, int16_t mtuSize);
    void Connect(const std::string &address, int port);
    void Start();
    void DisconnectAndStop();
    int Write(const ByteBuffer&);
    ByteBuffer Read();
    bool IsConnected();

    ByteBuffer Ping(const std::string &address, int port, Time deadline = defaultDeadline());
};

}  // namespace ProtoRock