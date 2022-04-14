#include "raknet.h"

#include "message.h"
#include "objectpool.h"

using namespace ProtoRock;
using namespace std::literals;
using namespace std::placeholders;

RaknetClient::RaknetClient(std::shared_ptr<CppServer::Asio::Service> service, int16_t mtuSize)
    : service(service), stop(false) {
}

void RaknetClient::Connect(const std::string &address, int port) {
    client = std::make_shared<UDPClient>(service, address, port);
    client->cbOnConnected = std::bind(&RaknetClient::onConnected, this);
    client->cbOnDisconnected = std::bind(&RaknetClient::onConnected, this);
    client->cbOnReceived = std::bind(&RaknetClient::onReceived, this, _1, _2, _3);
    client->cbOnError = std::bind(&RaknetClient::onError, this, _1, _2, _3);
    client->ConnectAsync();
}

void RaknetClient::DisconnectAndStop() {
    stop = true;

    connMtx.lock();
    if (connection) {
        connection->Disconnect();
    }
    connMtx.unlock();
    if (client) {
        client->DisconnectAsync();
    }
    while (IsConnected()) CppCommon::Thread::Yield();
}

void RaknetClient::onConnected() {
    std::cout << "Echo UDP client connected a new session with Id " << client->id() << std::endl;

    connMtx.lock();
    if (!connection) {  // New connection
        uint64_t guid;
        auto idBytes = client->id().data();
        uint8_t *_guid = (uint8_t *)&guid;
        for (int i = 0; i < 8; i++) {
            _guid[i] = idBytes[i];
        }
        connection = MakeState(client, client->endpoint(), guid);
    }
    connMtx.unlock();
}

void RaknetClient::onDisconnected() {
    std::cout << "Echo UDP client disconnected a session with Id " << client->id() << std::endl;

    connMtx.lock();
    connection->Disconnect();
    connection = nullptr;
    stop = true;
    connMtx.unlock();

    // Wait for a while...
    CppCommon::Thread::Sleep(1000);

    // Try to connect again
    if (!stop) client->ConnectAsync();
}

void RaknetClient::onReceived(const asio::ip::udp::endpoint &endpoint, const void *buffer, size_t size) {
    auto bb = ByteBufferFromCBuffer(buffer, size);
    connection->process(bb);
}

void RaknetClient::onError(int error, const std::string &category, const std::string &message) {
    std::cout << "Echo UDP client caught an error with code " << error << " and category '" << category << "': " << message << std::endl;
}

int RaknetClient::Write(const ByteBuffer &b) {
    if (connection) {
        return connection->Write(b);
    }
    throw RaknetException("not connected");
}

ByteBuffer RaknetClient::Read() {
    if (connection) {
        return connection->Read();
    }
    throw RaknetException("not connected");
}

bool RaknetClient::IsConnected() {
    if (connection) {
        return connection->IsConnected();
    }
    return false;
}

ByteBuffer RaknetClient::Ping(const std::string &address, int port, Time deadline) {
    std::atomic<bool> dataReady = false;
    bool errored = false;
    RaknetException error("");
    ByteBuffer data;

    auto u = std::make_shared<UDPClient>(service, address, port);
    u->cbOnConnected = [&]() {
        if (dataReady) {
            return;
        }
        try {
            auto p = UnconnectedPing();
            p.clientGUID = u->guid;
            p.sendTimestamp = timestamp();
            auto bb = PacketBuff();
            p.Write(bb);
            auto buff = bb.ToBuffer();
            u->Send(buff.data(), buff.size());
        } catch (RaknetException &e) {
            error = e;
            errored = true;
            dataReady = true;
        }
    };
    u->cbOnReceived = [&](const asio::ip::udp::endpoint& endpoint, const void* buffer, size_t size) {
        if (dataReady) {
            return;
        }
        try {
            auto bb = PacketBuff((const char *)buffer, size);
            bb.Read(); // discard id, assume pong
            auto p = UnconnectedPong();
            p.Read(bb);
            data = p.data;
            errored = false;
            dataReady = true;
        } catch(RaknetException &e) {
            error = e;
            errored = true;
            dataReady = true;
        }
    };

    if (!u->Connect()) {
        throw RaknetException("cannot connect to " + address);
    }

    bool deadlineExceeded = false;
    //std::cout << "Waiting for pong" << std::endl;
    // Wait for ping
    while (!dataReady) {
        CppCommon::Thread::Yield();
        if (Now() > deadline) {
            // Deadline exceeded
            deadlineExceeded = true;
            break;
        }
    }

    if (deadlineExceeded) {
        u->cbOnReceived = nullptr;
        u->cbOnConnected = nullptr;
        u->Disconnect();
        throw RaknetException("deadline exceeded");
    }

    // Clean-up
    u->cbOnReceived = nullptr;
    u->cbOnConnected = nullptr;
    u->Disconnect();

    if (errored) {
        throw error;
    }

    return data;
}