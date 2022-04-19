#pragma once

#include "common/crypto.h"
#include "oauth2/token.h"
#include "protocol/clientData.h"
#include "protocol/identityData.h"
#include "protocol/packet/encoder.h"
#include "protocol/packet/packet.h"
#include "raknet/raknet.h"

namespace ProtoRock {
namespace Minecraft {

class Client : public Protocol::Packet::EncoderOutput, public std::enable_shared_from_this<Client> {
   private:
    const ProtoRock::OAuth2::Token tkn;
    std::shared_ptr<RaknetClient> conn;
    std::mutex sendMtx;
    Crypto::KeyPair keyPair;
    Protocol::IdentityData identityData;
    Protocol::ClientData clientData;
    Protocol::Packet::PacketHeader header;
    std::vector<Common::ByteBuffer> bufferedSend;
    void Flush();
    std::shared_ptr<Client> getClient() { return shared_from_this(); }
    std::shared_ptr<Protocol::Packet::Encoder> encoder;

   public:
    Client(const ProtoRock::OAuth2::Token &tkn);
    ~Client();
    void WriteEncoded(const Common::ByteBuffer &b) override;
    void Connect(const std::string &serverAddress, int port);
    void WritePacket(const Protocol::Packet::Packet &p);
};

}  // namespace Minecraft
}  // namespace ProtoRock