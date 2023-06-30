#include "minecraft/client.h"

#include <chrono>

#include "common/objectpool.h"
#include "minecraft/login.h"
#include "oauth2/xbl.h"
#include "protocol/packet/allpackets.h"
#include "protocol/packet/packet.h"
#include "protocol/packet/packetPool.h"
#include "protocol/packet/reader.h"
#include "protocol/packet/writer.h"
#include "protocol/protocol.h"

using namespace std::literals;

using namespace ProtoRock;
using namespace ProtoRock::Minecraft;
using namespace ProtoRock::Common;
using namespace ProtoRock::Protocol;
using namespace ProtoRock::Protocol::Packet;

Client::Client(const ProtoRock::OAuth2::Token &tkn) : tkn(tkn), keyPair(Crypto::generateP384KeyPair()) {
    conn = std::make_shared<RaknetClient>(1400);
}

Client::~Client() {
    conn->DisconnectAndStop();
}

void Client::expect(std::vector<Protocol::Packet::PacketID> ids) {
    std::lock_guard<std::mutex> lock(expectMtx);
    expectedIds.clear();
    for (auto id : ids) {
        expectedIds[id] = id;
    }
}
bool Client::isExpected(Protocol::Packet::PacketID id) {
    std::lock_guard<std::mutex> lock(expectMtx);
    return expectedIds.count(id) > 0;
}

void Client::deferPacket(Protocol::Packet::IPacket packet) {
    std::lock_guard<std::mutex> lock(deferMtx);
    deferredPackets.push_back(packet);
}

Protocol::Packet::IPacket Client::takeDeferredPacket() {
    std::lock_guard<std::mutex> lock(deferMtx);
    if (deferredPackets.size() == 0) {
        return nullptr;
    }

    auto packet = deferredPackets.front();
    deferredPackets.pop_front();
    return packet;
}

void Client::WritePacket(const Protocol::Packet::Packet &pk) {
    std::lock_guard<std::mutex> lock(sendMtx);
    auto buff = ObjectPools::PacketBuffPool.Get();
    buff->resize(0);
    Protocol::Packet::Writer w(buff);

    header.packetId = pk.Id();
    header.Serialize(w);
    pk.Serialize(w);

    bufferedSend.push_back(buff->ToBuffer());

    buff->resize(0);
    ObjectPools::PacketBuffPool.Return(buff);
}

void Client::WriteEncoded(const Common::ByteBuffer &b) {
    std::cout << "Writing " << b.size() << " encoded bytes." << std::endl;
    conn->Write(b);
}

void Client::flush() {
    std::lock_guard<std::mutex> lock(sendMtx);
    if (bufferedSend.size()) {
        encoder->Encode(bufferedSend);
        bufferedSend.resize(0);
    }
}

void Client::receive(SharedByteBuffer b) {
    auto pbuff = ObjectPools::PacketBuffPool.Get();
    pbuff->resize(0);
    pbuff->insert(pbuff->begin(), b->begin(), b->end());
    ObjectPools::ByteBufferPool.Return(b);  // We already copied the data, so we can give the buffer back
    Protocol::Packet::PacketHeader header;
    Protocol::Packet::Reader r(pbuff);
    header.Deserialize(r);
    auto packet = Protocol::Packet::PacketPool::Pool.Get(header.packetId);
    packet->Deserialize(r);
    handlePacket(packet);
}

void Client::handlePacket(Protocol::Packet::IPacket packet) {
    fmt::print("-- received packet {}\n", packet->Id());
    if (packet->Id() == PacketID::Disconnect) {
        // Handle disconnect
        std::cout << "Received disconnect" << std::endl;
        Disconnect();
        return;
    }
    if (isExpected(packet->Id())) {
        handleInternalPacket(packet);
    } else {
        deferPacket(packet);
    }
}

void Client::Disconnect() {
    conn->DisconnectAndStop();
}

void Client::flushLoop() {
    while (true) {
        try {
            std::this_thread::sleep_for(50ms);
            flush();
        } catch (RaknetException &e) {
            std::cerr << "Received RaknetException: " << e.msg << std::endl;
            break;
        } catch (Exception &e) {
            std::cerr << "Error: " << e.msg << std::endl;
            break;
        }
    }
    std::cerr << "Flush loop closed" << std::endl;
}
void Client::listenLoop() {
    while (true) {
        try {
            auto packetData = conn->Read();
            auto packets = decoder->Decode(packetData);
            fmt::print("received {} packets\n", packets.size());
            for (auto pack : packets) {
                receive(pack);
            }
        } catch (RaknetException &e) {
            std::cerr << "Received RaknetException: " << e.msg << std::endl;
            break;
        } catch (Exception &e) {
            std::cerr << "Error: " << e.msg << std::endl;
            break;
        }
    }
    std::cerr << "Listen loop closed" << std::endl;
}

void Client::Connect(const std::string &serverAddress, int port) {
    if (!encoder) {
        encoder = std::make_shared<Protocol::Packet::Encoder>(getClient());
    }
    if (!decoder) {
        decoder = std::make_shared<Protocol::Packet::Decoder>();
    }

    // Ping Check
    auto pongData = conn->Ping(serverAddress, port);
    auto pong = std::string(pongData.begin(), pongData.end());
    std::cout << "PONG Data: " << pong << std::endl;
    auto chainData = std::string("");
    if (!tkn.AccessToken.empty()) {
        // Minecraft Chain Auth
        auto xblTkn = ProtoRock::OAuth2::RequestXBLToken(tkn, "https://multiplayer.minecraft.net/");
        chainData = ProtoRock::OAuth2::RequestMinecraftChain(keyPair, xblTkn);
    }

    conn->Connect(serverAddress, port);
    std::cout << "Waiting for Raknet Handshake" << std::endl;
    while (!conn->IsConnected()) {
        Common::Yield();
    }

    clientData.OS = Protocol::DeviceAndroid;
    clientData.GameVersion = Protocol::CurrentVersion;

    ProtoRock::Protocol::Packet::Login l;
    l.ClientProtocol = Protocol::CurrentProtocol;

    if (tkn.AccessToken.empty()) {
        // We don't have a XBL Token, so we make a self-signed one.
        l.ConnectionRequest = Login::EncodeOffline(identityData, clientData, keyPair);
    } else {
        l.ConnectionRequest = Login::Encode(chainData, clientData, keyPair);
        auto parsedLogin = Login::Parse(l.ConnectionRequest);
        identityData = parsedLogin.iData;
    }
    expect({PacketID::ServerToClientHandshake, PacketID::PlayStatus});
    WritePacket(l);
    flush();
    listenThread = std::thread(&Client::listenLoop, this);
    flushThread = std::thread(&Client::flushLoop, this);

    // Wait login
    while (!loggedInBefore) {
        Common::Yield();
    }
}