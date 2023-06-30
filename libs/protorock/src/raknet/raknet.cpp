#include "raknet.h"

#include <uuid.h>

#include <iostream>

#include "message.h"
#include "objectpool.h"
#include "protocol/packet/reader.h"
#include "protocol/packet/writer.h"

using namespace ProtoRock;
using namespace std::literals;
using namespace std::placeholders;

RaknetClient::RaknetClient(int16_t mtuSize)
    : stop(false),
      serviceStarted(false),
      ioService(std::make_shared<asio::io_service>()),
      mtuSize(mtuSize) {
    ioThread = std::thread(&RaknetClient::ioRoutine, this);
    while (!serviceStarted) {
        Common::Yield();
    }
    std::cout << "Service started" << std::endl;
}

void RaknetClient::Connect(const std::string &address, int port) {
    client = std::make_shared<UDPClient>(getService(), address, port);
    client->cbOnConnected = std::bind(&RaknetClient::onConnected, this);
    client->cbOnDisconnected = std::bind(&RaknetClient::onConnected, this);
    client->cbOnReceived = std::bind(&RaknetClient::onReceived, this, _1, _2);
    client->cbOnError = std::bind(&RaknetClient::onError, this, _1, _2, _3);
    client->Connect();
    connected = true;
}

void RaknetClient::ioRoutine() {
    asio::io_service::work work(*ioService);
    serviceStarted = true;
    ioService->run();
    std::cout << "IO Routine stopped" << std::endl;
}

void RaknetClient::DisconnectAndStop() {
    stop = true;

    connMtx.lock();
    if (connection) {
        connection->Disconnect();
    }
    connMtx.unlock();
    if (client) {
        client->Disconnect();
    }
    connected = false;
    ioService->stop();
}

void RaknetClient::onConnected() {
    std::cout << "Echo UDP client connected a new session with Id " << client->id.str() << std::endl;

    connMtx.lock();
    if (!connection) {  // New connection
        connection = MakeState(getRaknetClient(), client->remoteEndpoint, client->id.asUint64(), mtuSize);
    }
    connMtx.unlock();
}

void RaknetClient::onReceived(const asio::ip::udp::endpoint &endpoint, const ByteBuffer &b) {
    connection->process(b);
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

SharedByteBuffer RaknetClient::Read() {
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

    auto u = std::make_shared<UDPClient>(getService(), address, port);
    u->cbOnConnected = [&]() {
        if (dataReady) {
            return;
        }
        try {
            auto p = UnconnectedPing();
            p.clientGUID = u->id.asUint64();
            p.sendTimestamp = timestamp();
            auto bb = PacketBuff();
            p.Write(bb);
            u->Send(bb.ToBuffer());
        } catch (RaknetException &e) {
            error = e;
            errored = true;
            dataReady = true;
        }
    };
    u->cbOnReceived = [&](const asio::ip::udp::endpoint &endpoint, const ByteBuffer &b) {
        if (dataReady) {
            return;
        }
        try {
            auto bb = PacketBuff(b);
            bb.Read();  // discard id, assume pong
            auto p = UnconnectedPong();
            p.Read(bb);
            data = p.data;
            errored = false;
            dataReady = true;
        } catch (RaknetException &e) {
            error = e;
            errored = true;
            dataReady = true;
        }
    };

    u->Connect();

    bool deadlineExceeded = false;
    // std::cout << "Waiting for pong" << std::endl;
    //  Wait for ping
    while (!dataReady) {
        Common::Yield();
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