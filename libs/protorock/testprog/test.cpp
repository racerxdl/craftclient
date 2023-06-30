#include <common/crypto.h>
#include <jwt-cpp/jwt.h>
#include <oauth2/live.h>
#include <oauth2/xbl.h>
#include <raknet/raknet.h>
#include <minecraft/login.h>

#include <iostream>
#include <thread>
#include <protocol/packet/writer.h>
#include <protocol/packet/reader.h>
#include <minecraft/client.h>

using namespace std::literals;
using namespace ProtoRock;

int main() {
    OAuth2::Token tkn;
    bool gotToken = false;
    try {
        tkn = OAuth2::Token::LoadFromFile("livetoken.json");
        OAuth2::RefreshToken(tkn);
        gotToken = true;
        tkn.Save("livetoken.json");
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
            std::this_thread::sleep_for(1s);
        }
        std::cout << "Got live token!" << std::endl;
        tkn.Save("livetoken.json");
    }

    auto client = std::make_shared<Minecraft::Client>(tkn);

    client->Connect("10.10.5.43", 19000);

    std::this_thread::sleep_for (10s);

    return 0;
}