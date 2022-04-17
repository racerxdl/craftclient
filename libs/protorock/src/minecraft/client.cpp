#include "minecraft/client.h"
#include "oauth2/xbl.h"
#include "protocol/protocol.h"

using namespace ProtoRock;
using namespace ProtoRock::Minecraft;

Client::Client(const ProtoRock::OAuth2::Token &tkn) : tkn(tkn) {
    service = std::make_shared<CppServer::Asio::Service>();
    conn = std::make_shared<RaknetClient>(service, 1492);
    service->Start();
    keyPair = Crypto::generateES384KeyPair();
}

Client::~Client() {
    conn->DisconnectAndStop();
    service->Stop();
}

void Client::Connect(const std::string &serverAddress, int port) {
    // Ping Check
    auto pongData = conn->Ping(serverAddress, port);
    auto pong = std::string(pongData.begin(), pongData.end());
    std::cout << "PONG Data: " << pong << std::endl;

    // Minecraft Chain Auth
    auto xblTkn = ProtoRock::OAuth2::RequestXBLToken(keyPair, tkn, "https://multiplayer.minecraft.net/");
    auto chainData = ProtoRock::OAuth2::RequestMinecraftChain(keyPair, xblTkn);

    conn->Connect(serverAddress, port);
    clientData.OS = Protocol::DeviceAndroid;
    clientData.GameVersion = Protocol::CurrentVersion;


}