#pragma once

#include "common/crypto.h"
#include "oauth2/token.h"
#include "raknet/raknet.h"
#include "protocol/clientData.h"
#include "protocol/identityData.h"

namespace ProtoRock {
namespace Minecraft {

class Client {
   private:
    const ProtoRock::OAuth2::Token tkn;
    std::shared_ptr<RaknetClient> conn;
    std::shared_ptr<CppServer::Asio::Service> service;
    Crypto::KeyPair keyPair;
    Protocol::IdentityData identityData;
    Protocol::ClientData clientData;

   public:
    Client(const ProtoRock::OAuth2::Token &tkn);
    ~Client();
    void Connect(const std::string &serverAddress, int port);
};

}  // namespace Minecraft
}  // namespace ProtoRock