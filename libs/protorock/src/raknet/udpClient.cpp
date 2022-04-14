#include "udpClient.h"

using namespace ProtoRock;

UDPClient::UDPClient(std::shared_ptr<CppServer::Asio::Service> service, const std::string& address, uint16_t port)
    : CppServer::Asio::UDPClient(service, address, port) {

}

void UDPClient::onConnected() {
    uint64_t guid;
    auto idBytes = id().data();
    uint8_t* _guid = (uint8_t*)&guid;
    for (int i = 0; i < 8; i++) {
        _guid[i] = idBytes[i];
    }

    if (cbOnConnected) {
        cbOnConnected();
    }
    ReceiveAsync();
};
void UDPClient::onDisconnected() {
    if (cbOnDisconnected) {
        cbOnDisconnected();
    }
};
void UDPClient::onReceived(const asio::ip::udp::endpoint& endpoint, const void* buffer, size_t size) {
    if (cbOnReceived) {
        cbOnReceived(endpoint, buffer, size);
    }
    ReceiveAsync();
};
void UDPClient::onError(int error, const std::string& category, const std::string& message) {
    if (cbOnError) {
        cbOnError(error, category, message);
    }
};