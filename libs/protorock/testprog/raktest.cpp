#include <fmt/format.h>
#include <raknet/raknet.h>

#include <iostream>
#include <thread>

using namespace std::literals;
using namespace ProtoRock;

int main() {
    std::shared_ptr<RaknetClient> client = std::make_shared<RaknetClient>(1400);

    fmt::print("Connecting...\n");
    client->Connect("localhost", 1234);
    while (!client->IsConnected()) {
        Common::Yield();
    }
    fmt::print("Connected. waiting for payload\n");
    while (client->IsConnected()) {
        auto packet = client->Read();
        fmt::print("Received {} bytes\n", packet->size());
        std::string a(packet->begin(), packet->end());
        fmt::print("C: {}\n", a);
    }
    return 0;
}