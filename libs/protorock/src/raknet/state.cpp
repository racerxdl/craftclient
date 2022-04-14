#include "state.h"

#include "message.h"
#include "objectpool.h"

using namespace ProtoRock;
using namespace std::literals;

// currentProtocol is the current RakNet protocol version. This is Minecraft specific.
constexpr uint8_t currentProtocol = 10;

constexpr int maxMTUSize = 1400;
constexpr int maxWindowSize = 1024;

static ObjectPool<packet> packetPool;

state::state(std::shared_ptr<UDPClient> parent, UDPAddress remote, uint64_t guid, ConnMode mode)
    : timer(*parent->service()->GetAsioService(), asio::chrono::milliseconds(100)),
      mode(mode),
      parent(parent),
      guid(guid),
      remote(parent->endpoint()),
      currentState(DISCOVER_MTU_SIZE),
      mtuSize(maxMTUSize),
      stop(false) {
    this->packet = MakePacket();
    timer.expires_from_now(100ms);
    timer.async_wait(std::bind(&state::onTick, this, std::placeholders::_1));
}

void state::sendOpenConnectionRequest1(uint16_t mtu) {
    auto p = OpenConnectionRequest1();
    p.protocol = currentProtocol;
    p.maximumSizeNotDropped = mtu;
    auto b = PacketBuff();
    p.Write(b);
    auto buf = b.ToBuffer();
    buf.resize(mtu);
    parent->Send(buf.data(), buf.size());
}

void state::sendOpenConnectionRequest2(uint16_t mtu) {
    auto p = OpenConnectionRequest2();
    p.serverAddress = remote;
    p.clientGUID = guid;
    p.clientPreferredMTUSize = mtu;
    auto b = PacketBuff();
    p.Write(b);
    auto buf = b.ToBuffer();
    buf.resize(mtu);
    parent->Send(buf.data(), buf.size());
}

void state::onTick(const asio::system_error ec) {
    if (currentState == DISCOVER_MTU_SIZE) {
        sendOpenConnectionRequest1(mtuSize);
        mtuSize -= 40;
        if (!stop) {
            timer.expires_from_now(500ms);
            timer.async_wait(std::bind(&state::onTick, this, std::placeholders::_1));
        }
        return;
    }

    if (currentState == CONNECTING) {
        sendOpenConnectionRequest2(mtuSize);

        if (!stop) {
            timer.expires_from_now(500ms);
            timer.async_wait(std::bind(&state::onTick, this, std::placeholders::_1));
        }
        return;
    }

    try {
        flushACKs();

        if (!(loopCount % 40)) {
            sendPing();
        }

        if (!(loopCount % 3)) {
            checkResend(Now());
        }
    } catch (RaknetException &e) {
        std::cerr << "Received exception on tick: " << e.msg << std::endl;
    }

    loopCount++;
    if (!stop) {
        timer.expires_from_now(100ms);
        timer.async_wait(std::bind(&state::onTick, this, std::placeholders::_1));
    }
}

void state::flushACKs() {
    std::lock_guard<std::mutex> lock(datagramsRecvMtx);

    if (datagramsReceived.size() > 0) {
        try {
            sendACKs(datagramsReceived);
            datagramsReceived.clear();
        } catch (RaknetException &e) {
            throw e;
        }
    }
}
void state::sendPing() {
    PacketBuff buff;
    ConnectedPing p;
    p.ClientTimestamp = timestamp();
    p.Write(buff);
    Write(buff.ToBuffer());
}
void state::checkResend(Time t) {
    using namespace std::literals;

    std::vector<uint24_t> resendSeqNums;
    connectionMtx.lock();
    auto latency = recQueue.AvgDelay();
    this->latency = latency.count() / 2;
    auto delay = std::chrono::duration(3 * latency);
    delay += (latency / 2 + 50ms) * (resends + 1);

    for (const auto &pair : recQueue.timestamps) {
        if ((Now() - pair.second) > delay) {
            // std::cout << "Scheduled to resent " << pair.first << "because " << (Now() - pair.second).count() << " > " << delay.count() << std::endl;
            resendSeqNums.push_back(pair.first);
        }
    }

    auto l = resendSeqNums.size();
    resend(resendSeqNums);
    connectionMtx.unlock();

    if (l > 0) {
        resends += l;
        std::cout << "Resends: " << l << " - " << resends << std::endl;
        if (resends > 30) {
            Disconnect();
        }
    }
}
int state::Write(const ByteBuffer &b) {
    std::lock_guard<std::mutex> lock(connectionMtx);
    return write(b);
}

// Datagram header +
// Datagram sequence number +
// Packet header +
// Packet content length +
// Packet message index +
// Packet order index +
// Packet order channel
constexpr int packetAdditionalSize = 1 + 3 + 1 + 2 + 3 + 3 + 1;
// Packet split count +
// Packet split ID +
// Packet split index
constexpr int splitAdditionalSize = 4 + 2 + 4;

std::vector<ByteBuffer> state::split(const ByteBuffer &b) {
    auto maxSize = (mtuSize - packetAdditionalSize) - 28;
    auto contentLength = b.size();

    if (contentLength > maxSize) {
        // If the content size is bigger than the maximum size here, it means the packet will get split. This
        // means that the packet will get even bigger because a split packet uses 4 + 2 + 4 more bytes.
        maxSize -= splitAdditionalSize;
    }

    auto fragmentCount = contentLength / maxSize;
    if (contentLength % maxSize != 0) {
        fragmentCount++;
    }

    std::vector<ByteBuffer> fragments;
    int pos = 0;
    for (int i = 0; i < fragmentCount; i++) {
        auto start = b.begin() + pos;
        auto end = start + maxSize;
        if (end > b.end()) {
            end = b.end();
        }
        fragments.push_back(ByteBuffer(start, end));
        pos += end - start;
    }
    return fragments;
}

int state::write(const ByteBuffer &b) {
    auto n = 0;
    auto fragments = split(b);
    auto orderIndex = this->orderIndex;
    this->orderIndex++;

    auto splitId = (uint16_t)this->splitId;
    auto split = fragments.size() > 1;
    if (split) {
        this->splitId++;
    }
    int splitIndex = 0;
    // std::cout << "There are " << fragments.size() << " fragments to send" << std::endl;
    for (const auto &content : fragments) {
        auto sequenceNumber = this->seq;
        this->seq++;
        auto messageIndex = this->messageIndex;
        this->messageIndex++;

        buf.Write(bitFlagDatagram);
        buf.WriteUint24(sequenceNumber);
        auto pk = packetPool.Get();
        pk->reliability = reliabilityReliableOrdered;
        pk->content.resize(0);
        pk->content.insert(pk->content.end(), content.begin(), content.end());
        pk->orderIndex = orderIndex;
        pk->messageIndex = messageIndex;
        pk->split = split;
        if (split) {
            pk->splitCount = fragments.size();
            pk->splitIndex = splitIndex;
            pk->splitId = splitId;
        }
        pk->write(buf);
        auto b = buf.ToBuffer();
        //std::cout << "Sending " << b.size() << " bytes" << std::endl;
        parent->Send(remote, b.data(), b.size());
        buf.clear();

        recQueue.put(sequenceNumber, pk);
        n += content.size();
        splitIndex++;
    }

    return n;
}

ByteBuffer state::Read() {
    if (!IsConnected()) {
        throw RaknetException("not connected");
    }
    // TODO: Timeout
    auto p = receivedPackets.Get();
    while (!p.second) {
        CppCommon::Thread::Yield();
        if (!IsConnected()) {
            throw RaknetException("not connected");
        }
        p = receivedPackets.Get();
    }
    return p.first;
}

void state::receive(const ByteBuffer &_b) {
    PacketBuff b(_b);
    auto headerflags = b.Read();
    if ((headerflags & bitFlagDatagram) == 0) {
        // Ignore non-datagram packets
        return;
    }

    if ((headerflags & bitFlagACK) != 0) {
        handleACK(b);
        return;
    }
    if ((headerflags & bitFlagNACK) != 0) {
        handleNACK(b);
        return;
    }

    receiveDatagram(b);
}

void state::receiveDatagram(PacketBuff &b) {
    auto sequenceNumber = b.ReadUint24();

    datagramsRecvMtx.lock();
    datagramsReceived.push_back(sequenceNumber);
    datagramsRecvMtx.unlock();

    auto lowestBefore = dataQueue.lowest;
    if (!dataQueue.put(sequenceNumber)) {
        throw RaknetException("error handling datagram: packet already received");
    }
    dataQueue.clear();

    if (dataQueue.lowest == lowestBefore) {
        missingDatagramTimes++;
        if (missingDatagramTimes >= 10) {
            sendNACKs(dataQueue.missing());
            missingDatagramTimes = 0;
        }
    } else {
        missingDatagramTimes = 0;
    }

    if (dataQueue.windowSize() > maxWindowSize) {
        throw RaknetException("datagram receive queue window size is too big");
    }

    while (b.size() > 0) {
        packet->read(b);
        if (packet->split) {
            handleSplitPacket(packet);
        } else {
            receivePacket(packet);
        }
    }
}

void state::handleSplitPacket(Packet p) {
    constexpr int maxSplitCount = 256;
    if (p->splitCount > maxSplitCount || splits.size() > 64) {
        throw RaknetException("maximum active split count reached");
    }

    if (!splits.count(p->splitId)) {
        splits[p->splitId] = std::vector<ByteBuffer>();
        splits[p->splitId].resize(p->splitCount);
    }

    auto m = splits[p->splitId];
    if (p->splitIndex > m.size() - 1) {
        throw RaknetException("error handing split packet: splitIndex out of range");
    }
    m[p->splitIndex] = p->content.ToBuffer();

    auto size = 0;
    for (auto &fragment : m) {
        if (fragment.size() == 0) {
            return;  // Not all fragments received
        }
        size += fragment.size();
    }

    auto content = ByteBuffer();
    content.reserve(size);
    for (auto &fragment : m) {
        content.insert(content.end(), fragment.begin(), fragment.end());
    }
    splits.erase(p->splitId);
    p->content = content;
    return receivePacket(p);
}

void state::receivePacket(Packet p) {
    auto b = p->content.ToBuffer();
    if (p->reliability != reliabilityReliableOrdered) {
        handlePacket(b);
        return;
    }

    if (!packQueue.put(p->orderIndex, b)) {
        // An ordered packet arrived twice;
        return;
    }

    auto f = packQueue.fetch();
    for (auto &content : f) {
        handlePacket(content);
    }
}

void state::handlePacket(ByteBuffer &p) {
    auto b = PacketBuff(p);
    auto id = b.Read();
    // std::cout << "Received packet " << (int)id << std::endl;
    switch (id) {
        case IDConnectionRequest:
            handleConnectionRequest(b);
            return;
        case IDConnectionRequestAccepted:
            handleConnectionRequestAccepted(b);
            return;
        case IDNewIncomingConnection:
            // Not sure what to do
            return;
        case IDConnectedPing:
            handleConnectedPing(b);
            return;
        case IDConnectedPong:
            handleConnectedPong(b);
            return;
        case IDDisconnectNotification:
            Disconnect();
            return;
        case IDDetectLostConnections:
            sendPing();
            return;
        default:
            b.push_front(id);
            // Passthrough data
            receivedPackets.Put(b.ToBuffer());
    }
}

void state::sendAcknowledgement(const std::vector<uint24_t> ids, uint8_t flag, PacketBuff b) {
    acknowledgement ack;
    ack.packets = ids;

    while (ack.packets.size() != 0) {
        b.resize(0);
        b.Write(flag | bitFlagDatagram);
        ack.write(b, mtuSize);
        ack.packets.erase(ack.packets.begin());
        auto buf = b.ToBuffer();
        parent->Send(remote, buf.data(), buf.size());
    }
    b.resize(0);
}

void state::sendNACKs(const std::vector<uint24_t> ids) {
    sendAcknowledgement(ids, bitFlagNACK, nackBuf);
    nackBuf.clear();
}

void state::sendACKs(const std::vector<uint24_t> ids) {
    sendAcknowledgement(ids, bitFlagACK, ackBuf);
    ackBuf.clear();
}

void state::handleACK(PacketBuff &b) {
    std::lock_guard<std::mutex> lock(connectionMtx);

    auto ack = acknowledgement();
    ack.read(b);
    auto success = 0;
    for (auto &sequenceNumber : ack.packets) {
        // std::cout << "Receive ACK for packet " << sequenceNumber << std::endl;
        auto p = recQueue.take(sequenceNumber);
        if (p != nullptr) {
            p->content.resize(0);
            packetPool.Return(p);
            success++;
        }
    }
    if (resends > 0) {
        resends -= 1;
    }
}

void state::handleNACK(PacketBuff &b) {
    std::lock_guard<std::mutex> lock(connectionMtx);
    auto nack = acknowledgement();
    nack.read(b);
    resend(nack.packets);
}

void state::handleConnectedPing(PacketBuff &b) {
    auto packet = ConnectedPing();
    packet.Read(b);
    b.clear();

    auto pong = ConnectedPong();
    pong.ClientTimestamp = packet.ClientTimestamp;
    pong.ServerTimestamp = timestamp();
    pong.Write(b);
    Write(b.ToBuffer());
}

void state::handleConnectedPong(PacketBuff &b) {
    auto packet = ConnectedPong();
    packet.Read(b);
    b.clear();
    if (packet.ClientTimestamp > timestamp()) {
        throw RaknetException("error measuring latency: ping timestamp is in the future");
    }
    // std::cout << "Received PONG" << std::endl;
    // Nothing to do
}

void state::handleConnectionRequest(PacketBuff &b) {
    auto packet = ConnectionRequest();
    packet.Read(b);
    b.clear();

    auto resp = ConnectionRequestAccepted();
    resp.clientAddress = local;
    resp.RequestTimestamp = packet.RequestTimestamp;
    resp.AcceptedTimestamp = timestamp();
    resp.Write(b);
    Write(b.ToBuffer());
}

void state::handleConnectionRequestAccepted(PacketBuff &b) {
    auto packet = ConnectionRequestAccepted();
    packet.Read(b);
    b.clear();

    auto resp = NewIncomingConnection();
    resp.serverAddress = local;
    resp.RequestTimestamp = packet.RequestTimestamp;
    resp.AcceptedTimestamp = packet.AcceptedTimestamp;
    for (int i = 0; i < 20; i++) {
        resp.systemAddresses[i] = packet.systemAddresses[i];
    }
    resp.Write(b);
    Write(b.ToBuffer());
    b.clear();

    connected = true;
}

void state::requestConnection() {
    auto p = ConnectionRequest();
    p.ClientGUID = guid;
    p.RequestTimestamp = timestamp();
    auto b = PacketBuff();
    p.Write(b);
    Write(b.ToBuffer());
}

void state::resend(std::vector<uint24_t> sequenceNumbers) {
    buf.resize(0);
    for (auto sequenceNumber : sequenceNumbers) {
        auto p = recQueue.takeWithoutDelayAdd(sequenceNumber);
        buf.Write(bitFlagDatagram);
        auto newSeqNum = seq;
        seq++;
        buf.WriteUint24(newSeqNum);
        p->write(buf);
        auto b = buf.ToBuffer();
        parent->Send(remote, b.data(), b.size());
        recQueue.put(newSeqNum, p);
        buf.resize(0);
    }
}

void state::Disconnect() {
    std::lock_guard<std::mutex> lock(connectionMtx);
    std::cout << "Remove disconnected" << std::endl;
    stop = true;

    if (connected) {
        auto p = ByteBuffer();
        p.push_back(IDDisconnectNotification);
        write(p);
    }
    connected = false;
    if (mode == CLIENT) {
        parent->Disconnect();
    }

    currentState = DISCOVER_MTU_SIZE;
    isInvalid = true;
}

void state::process(ByteBuffer &bb) {
    if (currentState == DISCOVER_MTU_SIZE) {
        auto b = PacketBuff(bb);
        auto id = b.Read();
        auto resp1 = OpenConnectionReply1();
        auto resp2 = IncompatibleProtocolVersion();
        switch (id) {
            case IDOpenConnectionReply1:
                resp1.Read(b);
                if (resp1.serverPreferredMTUSize < 400 || resp1.serverPreferredMTUSize > 1500) {
                    sendOpenConnectionRequest2(resp1.serverPreferredMTUSize);
                    mtuSize += 40;
                } else {
                    mtuSize = resp1.serverPreferredMTUSize;
                    // std::cout << "Switching to CONNECTING. MTUSIZE: " << mtuSize << std::endl;
                    currentState = CONNECTING;
                }
                break;
            case IDIncompatibleProtocolVersion:
                resp2.Read(b);
                std::cerr << "Incompatible protocol. My protocol: " << currentProtocol << " - server protocol: " << resp2.serverProtocol << std::endl;
                Disconnect();
                return;
        }
        return;
    }

    if (currentState == CONNECTING) {
        auto b = PacketBuff(bb);
        auto id = b.Read();
        if (id == IDOpenConnectionReply2) {
            auto resp = OpenConnectionReply2();
            resp.Read(b);
            mtuSize = resp.mtuSize;
            currentState = NORMAL;
            // std::cout << "Switching to NORMAL. MTUSize: " << mtuSize << std::endl;
            requestConnection();
        }
        return;
    }
    if (currentState == NORMAL) {
        try {
            receive(bb);
        } catch (RaknetException &e) {
            std::cerr << "Error handling received packet: " << e.msg << std::endl;
        }
    }
}