#include "udpClient.h"

using namespace ProtoRock;

UDPClient::UDPClient(std::shared_ptr<asio::io_service> service, const std::string& address, uint16_t port)
    : ioService(service), socket(*ioService), id(Common::UUID::NewRandom()) {
    asio::ip::udp::resolver::query query(address, std::to_string(port));
    asio::ip::udp::resolver resolver(*ioService);
    remoteEndpoint = *resolver.resolve(query);
    rxbuffer.resize(1500);
    receiving = false;
}

void UDPClient::Connect() {
    localEndpoint = asio::ip::udp::endpoint(udp::v4(), 0);
    socket.open(udp::v4());
    socket.bind(localEndpoint);
    onConnected();
}

void UDPClient::ReceiveAsync() {
    if (receiving) {
        return;
    }
    if (!socket.is_open()) {
        return;
    }
    receiving = true;
    rxbuffer.resize(1500); // Restore size
    auto self = getUDPClient();
    auto asyncReceiveHandler = [this, self](std::error_code ec, size_t size) {
        receiving = false;
        rxbuffer.resize(size);  // Shrink without changing capacity so we dont reallocate

        if (ec) {
            // Skip Asio disconnect errors
            if ((ec == asio::error::connection_aborted) ||
                (ec == asio::error::connection_refused) ||
                (ec == asio::error::connection_reset) ||
                (ec == asio::error::eof) ||
                (ec == asio::error::operation_aborted)) {
                return;
            }
            this->onError(ec.value(), ec.category().name(), ec.message());
            this->onDisconnected();
            return;
        }

        this->onReceived(remoteEndpoint, rxbuffer);
    };
    socket.async_receive_from(asio::buffer(rxbuffer.data(), rxbuffer.size()), remoteEndpoint, asyncReceiveHandler);
}

void UDPClient::onConnected() {
    if (cbOnConnected) {
        cbOnConnected();
    }
    ReceiveAsync();
}

void UDPClient::onDisconnected() {
    socket.close();
    if (cbOnDisconnected) {
        cbOnDisconnected();
    }
}

void UDPClient::onReceived(const asio::ip::udp::endpoint& endpoint, const Common::ByteBuffer& b) {
    // std::cout << "Received " << b.size() << " bytes from " << endpoint << std::endl;
    if (cbOnReceived) {
        cbOnReceived(endpoint, b);
    }
    ReceiveAsync();
}

void UDPClient::onError(int error, const std::string& category, const std::string& message) {
    if (cbOnError) {
        cbOnError(error, category, message);
    }
}

void UDPClient::SendTo(const asio::ip::udp::endpoint& endpoint, const Common::ByteBuffer& b) {
    if (!socket.is_open()) {
        return;
    }
    if (b.size() == 0) {
        return;
    }
    asio::error_code ec;
    socket.send_to(asio::buffer(b), endpoint, 0, ec);
    if (ec) {
        // Skip Asio disconnect errors
        if ((ec == asio::error::connection_aborted) ||
            (ec == asio::error::connection_refused) ||
            (ec == asio::error::connection_reset) ||
            (ec == asio::error::eof) ||
            (ec == asio::error::operation_aborted)) {
            return;
        }
        onError(ec.value(), ec.category().name(), ec.message());
        onDisconnected();
    }
}