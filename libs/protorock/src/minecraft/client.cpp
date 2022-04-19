#include "minecraft/client.h"

#include "oauth2/xbl.h"
#include "protocol/protocol.h"
#include "minecraft/login.h"

using namespace ProtoRock;
using namespace ProtoRock::Minecraft;

Client::Client(const ProtoRock::OAuth2::Token &tkn) : tkn(tkn) {
    conn = std::make_shared<RaknetClient>(1492);
    keyPair = Crypto::generateES384KeyPair();
}

Client::~Client() {
    conn->DisconnectAndStop();
}

void Client::Connect(const std::string &serverAddress, int port) {
    // Ping Check
    auto pongData = conn->Ping(serverAddress, port);
    auto pong = std::string(pongData.begin(), pongData.end());
    std::cout << "PONG Data: " << pong << std::endl;
    auto chainData = std::string("");
    if (!tkn.AccessToken.empty()) {
        // Minecraft Chain Auth
        auto xblTkn = ProtoRock::OAuth2::RequestXBLToken(keyPair, tkn, "https://multiplayer.minecraft.net/");
        chainData = ProtoRock::OAuth2::RequestMinecraftChain(keyPair, xblTkn);
    }

    conn->Connect(serverAddress, port);
    clientData.OS = Protocol::DeviceAndroid;
    clientData.GameVersion = Protocol::CurrentVersion;

    ByteBuffer request;

    if (tkn.AccessToken.empty()) {
        // We don't have a XBL Token, so we make a self-signed one.
        request = Login::EncodeOffline(identityData, clientData, keyPair);
    } else {
        request = Login::Encode(chainData, clientData, keyPair);
        auto parsedLogin = Login::Parse(request);
        identityData = parsedLogin.iData;
    }
}