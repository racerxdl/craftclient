#pragma once

#include <server/asio/timer.h>
#include <server/asio/udp_client.h>

#include <atomic>
#include <cstdint>
#include <exception>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "common.h"
#include "udpClient.h"
#include "datagramQueue.h"
#include "packetQueue.h"
#include "recoveryQueue.h"
#include "safeQueue.h"

namespace ProtoRock {

constexpr int MaxBufferPackets = 64;

enum ConnState {
    DISCOVER_MTU_SIZE,
    CONNECTING,
    NORMAL,
};

enum ConnMode {
    CLIENT,
    SERVER
};

struct state {
   private:
    ConnState currentState = DISCOVER_MTU_SIZE;
    ConnMode mode = CLIENT;
    std::atomic<bool> connected = false;
    std::atomic<bool> isInvalid = false;
    uint64_t guid = 0;
    std::mutex datagramsRecvMtx;
    std::mutex connectionMtx;
    std::atomic<uint32_t> latency = 0;

    datagramQueue dataQueue;
    recoveryQueue recQueue;
    packetQueue packQueue;
    SafeQueue<ByteBuffer> receivedPackets;
    std::vector<uint24_t> datagramsReceived;
    std::atomic<uint32_t> missingDatagramTimes = 0;

    asio::steady_timer timer;
    std::atomic<uint16_t> mtuSize = 1400;
    std::atomic<uint32_t> resends = 0;
    uint64_t loopCount = 0;

    uint24_t seq = 0, orderIndex = 0, messageIndex = 0;
    uint32_t splitId = 0;
    Packet packet;
    PacketBuff buf, ackBuf, nackBuf;
    UDPAddress local, remote;

    std::queue<ByteBuffer> packets;
    std::unordered_map<uint16_t, std::vector<ByteBuffer>> splits;

    std::shared_ptr<UDPClient> parent;

    // MTU Discover
    void sendOpenConnectionRequest1(uint16_t);
    void sendOpenConnectionRequest2(uint16_t);

    // Raknet
    void flushACKs();
    void sendPing();
    void checkResend(Time);
    void sendAcknowledgement(const std::vector<uint24_t> ids, uint8_t flag, PacketBuff b);
    void sendACKs(const std::vector<uint24_t> ids);
    void sendNACKs(const std::vector<uint24_t> ids);
    void resend(std::vector<uint24_t>);
    int write(const ByteBuffer &);
    std::vector<ByteBuffer> split(const ByteBuffer &);
    void receive(const ByteBuffer &);
    void receiveDatagram(PacketBuff &);
    void receivePacket(Packet);
    void handleACK(PacketBuff &);
    void handleNACK(PacketBuff &);
    void handlePacket(ByteBuffer &);
    void handleSplitPacket(Packet);
    void requestConnection();

    // Raknet Flow Handlers
    void handleConnectedPing(PacketBuff &);
    void handleConnectedPong(PacketBuff &);
    void handleConnectionRequest(PacketBuff &);
    void handleConnectionRequestAccepted(PacketBuff &);

    // Ticker
    void onTick(const asio::system_error ec);
    std::atomic<bool> stop;

   public:
    void process(ByteBuffer &);
    int Write(const ByteBuffer &);
    ByteBuffer Read();
    void Disconnect();
    bool IsConnected() { return connected; }
    bool IsInvalid() { return isInvalid; }

    state(std::shared_ptr<UDPClient> parent, UDPAddress remote, uint64_t guid, ConnMode mode = CLIENT);
};
typedef std::shared_ptr<state> State;

inline State MakeState(std::shared_ptr<UDPClient> parent, UDPAddress remote, uint64_t guid, ConnMode mode = CLIENT) {
    return std::make_shared<state>(parent, remote, guid, mode);
}
}  // namespace ProtoRock