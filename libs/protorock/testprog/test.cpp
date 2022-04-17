#include <raknet/raknet.h>
#include <oauth2/live.h>
#include <oauth2/xbl.h>
#include <common/crypto.h>

#include <iostream>
#include <thread>

using namespace std::literals;
using namespace ProtoRock;

int main() {
    auto kp = ProtoRock::Crypto::generateES256KeyPair();
    auto pub = kp.PKIXPublicKey();

    std::cout << Base64::Encode(pub) << std::endl;

    auto service = std::make_shared<CppServer::Asio::Service>();

    service->Start();
    OAuth2::Token tkn;
    bool gotToken = false;
    try {
        tkn = OAuth2::Token::LoadFromFile("livetoken.json");
        gotToken = true;
    } catch (Exception &e) {
        std::cout << e.msg << ". Getting new token..." << std::endl;
    }

    if (!gotToken) {
        auto d = ProtoRock::OAuth2::StartDeviceAuth();
        std::cout << fmt::format("Please login in {} and use code {}", d.VerificationURI, d.UserCode) << std::endl;
        while (true) {
            if (ProtoRock::OAuth2::PollDeviceAuth(d.DeviceCode, tkn)) {
                break;
            }
            std::this_thread::sleep_for (1s);
        }
        std::cout << "Got live token!" << std::endl;
        tkn.Save("livetoken.json");
    }
    ProtoRock::OAuth2::XBLToken xblTkn;
    gotToken = false;

    // try {
    //     xblTkn = ProtoRock::OAuth2::XBLToken::LoadFromFile("xbltoken.json");
    //     gotToken = true;
    // } catch (Exception &e) {
    //     std::cout << e.msg << ". Getting new token..." << std::endl;
    // }
    std::cout << "Requesting XBL Token" << std::endl;
    // if (!gotToken) {
    xblTkn = ProtoRock::OAuth2::RequestXBLToken(kp, tkn, "https://multiplayer.minecraft.net/");
    xblTkn.Save("xbltoken.json");
    // }

    auto rawData = ProtoRock::OAuth2::RequestMinecraftChain(kp, xblTkn);

    std::cout << "Raw Data: " << rawData << std::endl;
    std::ofstream f;
    f.open("rawData.txt");
    f << rawData;
    f.close();

    std::cout << "DONE!" << std::endl;

    return -1;

    auto raknet = std::make_shared<RaknetClient>(service, 1492);
    try {
        auto pongData = raknet->Ping("127.0.0.1", 19132);
        std::stringstream ss;
        for (int i = 0; i < pongData.size(); i++) {
            ss << (unsigned char)pongData[i];
        }
        std::cout << "Pong data: " << ss.str() << std::endl;
    } catch (RaknetException e) {
        std::cerr << "Error pinging: " << e.msg << std::endl;
        return 1;
    }

    CppCommon::Thread::Sleep(5000);
    raknet->Connect("127.0.0.1", 19132);

    // Wait connection
    std::cout << "Waiting connection" << std::endl;
    while (true) {
        if (raknet->IsConnected()) {
            break;
        }
        CppCommon::Thread::Yield();
    }
    std::cout << "Connected! Waiting packets" << std::endl;
    while (true) {
        if (!raknet->IsConnected()) {
            break;
        }
        try {
            auto p = raknet->Read();
            std::stringstream ss;
            for (int i = 0; i < p.size(); i++) {
                ss << (unsigned char)p[i];
            }
            std::cout << "Received packet: " << ss.str() << std::endl;
        } catch(RaknetException &e) {
            std::cerr << "RaknetException receiving packet: " << e.msg << std::endl;
        }
        CppCommon::Thread::Yield();
    }
    std::cout << "Disconnected. Closing..." << std::endl;
    raknet->DisconnectAndStop();
    service->Stop();

    return 0;
}