#include <raknet/raknet.h>

#include <iostream>

using namespace ProtoRock;

int main() {
    auto service = std::make_shared<CppServer::Asio::Service>();

    service->Start();

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