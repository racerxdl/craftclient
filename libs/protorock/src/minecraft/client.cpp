#include "minecraft/client.h"

#include "common/objectpool.h"
#include "minecraft/login.h"
#include "oauth2/xbl.h"
#include "protocol/packet/login.h"
#include "protocol/packet/reader.h"
#include "protocol/packet/writer.h"
#include "protocol/protocol.h"

using namespace ProtoRock;
using namespace ProtoRock::Minecraft;
using namespace ProtoRock::Common;
using namespace ProtoRock::Protocol;

ByteBuffer NullByte() {
    auto b = ByteBuffer();
    b.push_back(0);
    return b;
}

Client::Client(const ProtoRock::OAuth2::Token &tkn) : tkn(tkn), keyPair(Crypto::generateP384KeyPair()) {
    conn = std::make_shared<RaknetClient>(1492);
}

Client::~Client() {
    conn->DisconnectAndStop();
}

void Client::WritePacket(const Protocol::Packet::Packet &pk) {
    std::lock_guard<std::mutex> lock(sendMtx);
    auto buff = ObjectPools::PacketBuffPool.Get();
    Protocol::Packet::Writer w(buff);

    header.packetId = pk.Id();
    header.Serialize(w);
    pk.Serialize(w);

    bufferedSend.push_back(NullByte());
    bufferedSend.push_back(buff->ToBuffer());

    buff->resize(0);
    ObjectPools::PacketBuffPool.Return(buff);
}

void Client::WriteEncoded(const Common::ByteBuffer &b) {
    conn->Write(b);
}

void Client::Flush() {
    std::lock_guard<std::mutex> lock(sendMtx);
    if (bufferedSend.size()) {
        encoder->Encode(bufferedSend);
        bufferedSend.resize(0);
    }
}

void Client::Connect(const std::string &serverAddress, int port) {
    if (!encoder) {
        encoder = std::make_shared<Protocol::Packet::Encoder>(getClient());
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
    clientData.OS = Protocol::DeviceAndroid;
    clientData.GameVersion = Protocol::CurrentVersion;

    ProtoRock::Protocol::Packet::Login l;

    if (tkn.AccessToken.empty()) {
        // We don't have a XBL Token, so we make a self-signed one.
        l.ConnectionRequest = Login::EncodeOffline(identityData, clientData, keyPair);
    } else {
        l.ConnectionRequest = Login::Encode(chainData, clientData, keyPair);
        auto parsedLogin = Login::Parse(l.ConnectionRequest);
        identityData = parsedLogin.iData;
    }

    WritePacket(l);
    Flush();
}