#pragma once

#include <asio.hpp>
#include <memory>

#include "common.h"

namespace ProtoRock {
    using asio::ip::udp;
    class UDPClient : public std::enable_shared_from_this<UDPClient>  {
    private:
        Common::ByteBuffer rxbuffer;
        std::shared_ptr<asio::io_service> ioService;
        udp::socket socket;
        std::atomic<bool> receiving;

        void onConnected();
        void onDisconnected();
        void onReceived(const asio::ip::udp::endpoint& endpoint, const Common::ByteBuffer &b);
        void onError(int error, const std::string& category, const std::string& message);
        void ReceiveAsync();

        std::shared_ptr<UDPClient> getUDPClient() { return shared_from_this(); }
    public:
        Common::UUID id;
        udp::endpoint remoteEndpoint;
        udp::endpoint localEndpoint;
        std::function<void()> cbOnConnected;
        std::function<void()> cbOnDisconnected;
        std::function<void(const asio::ip::udp::endpoint& endpoint, const Common::ByteBuffer &b)> cbOnReceived;
        std::function<void(int error, const std::string& category, const std::string& message)> cbOnError;

        void Send(const Common::ByteBuffer &b) { SendTo(remoteEndpoint, b); }
        void SendTo(const asio::ip::udp::endpoint& endpoint, const Common::ByteBuffer &b);
        void Disconnect() { socket.close(); }
        void Connect();

        UDPClient(std::shared_ptr<asio::io_service> service, const std::string &address, uint16_t port);
    };
}