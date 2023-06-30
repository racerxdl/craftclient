#pragma once

#include "common/crypto.h"
#include "oauth2/token.h"
#include "protocol/clientData.h"
#include "protocol/identityData.h"
#include "protocol/packet/encoder.h"
#include "protocol/packet/decoder.h"
#include "protocol/packet/packet.h"
#include "protocol/packet/allpackets.h"
#include "raknet/raknet.h"

namespace ProtoRock {
namespace Minecraft {

class Client : public Protocol::Packet::EncoderOutput, public std::enable_shared_from_this<Client> {
   private:
    std::mutex sendMtx;
    std::mutex expectMtx;
    std::mutex deferMtx;

    const ProtoRock::OAuth2::Token tkn;
    std::shared_ptr<RaknetClient> conn = nullptr;
    Crypto::KeyPair keyPair;
    Protocol::IdentityData identityData;
    Protocol::ClientData clientData;
    Protocol::Packet::PacketHeader header;
    std::vector<Common::ByteBuffer> bufferedSend;
    std::shared_ptr<Protocol::Packet::Encoder> encoder = nullptr;
    std::shared_ptr<Protocol::Packet::Decoder> decoder = nullptr;
    std::thread listenThread;
    std::thread flushThread;
    std::atomic<bool> loggedInBefore = false;
    std::unordered_map<Protocol::Packet::PacketID, Protocol::Packet::PacketID> expectedIds;
    Protocol::Packet::PacketList deferredPackets;


    std::shared_ptr<Client> getClient() { return shared_from_this(); }
    void expect(std::vector<Protocol::Packet::PacketID> ids);
    bool isExpected(Protocol::Packet::PacketID id);
    void flush();
    void listenLoop();
    void flushLoop();
    void receive(SharedByteBuffer b);
    void handlePacket(Protocol::Packet::IPacket pkt);
    void handleInternalPacket(Protocol::Packet::IPacket pkt);
    void deferPacket(Protocol::Packet::IPacket packet);
    Protocol::Packet::IPacket takeDeferredPacket();

    // Packet Handlers
    void handleLogin(Protocol::Packet::Login packet);
    void handleServerToClientHandshake(Protocol::Packet::ServerToClientHandshake packet);
    void handlePlayStatus(Protocol::Packet::PlayStatus packet);
    void handleResourcePacksInfo(Protocol::Packet::ResourcePacksInfo packet);
    void handleResourcePackStack(Protocol::Packet::ResourcePackStack packet);
   public:
    Client(const ProtoRock::OAuth2::Token &tkn);
    ~Client();
    void Disconnect();
    void WriteEncoded(const Common::ByteBuffer &b) override;
    void Connect(const std::string &serverAddress, int port);
    void WritePacket(const Protocol::Packet::Packet &p);
};

}  // namespace Minecraft
}  // namespace ProtoRock