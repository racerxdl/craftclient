#pragma once

#include <server/asio/udp_client.h>

namespace ProtoRock {
    class UDPClient : public CppServer::Asio::UDPClient {
    private:
        void onConnected() override;
        void onDisconnected() override;
        void onReceived(const asio::ip::udp::endpoint& endpoint, const void* buffer, size_t size) override;
        void onError(int error, const std::string& category, const std::string& message) override;
    public:
        uint64_t guid;
        std::function<void()> cbOnConnected;
        std::function<void()> cbOnDisconnected;
        std::function<void(const asio::ip::udp::endpoint& endpoint, const void* buffer, size_t size)> cbOnReceived;
        std::function<void(int error, const std::string& category, const std::string& message)> cbOnError;
        UDPClient(std::shared_ptr<CppServer::Asio::Service> service, const std::string &address, uint16_t port);
    };
}