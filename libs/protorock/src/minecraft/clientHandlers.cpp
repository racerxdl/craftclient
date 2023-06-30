#include <fmt/format.h>

#include <chrono>

#include "common/objectpool.h"
#include "minecraft/client.h"
#include "protocol/packet/allpackets.h"
#include "protocol/packet/id.h"
#include "protocol/packet/packetPool.h"
#include "protocol/protocol.h"

using namespace std::literals;

using namespace ProtoRock;
using namespace ProtoRock::Minecraft;
using namespace ProtoRock::Common;
using namespace ProtoRock::Protocol;
using namespace ProtoRock::Protocol::Packet;

void Client::handleInternalPacket(Protocol::Packet::IPacket packet) {
    bool handled = false;
    switch (packet->Id()) {
            // Internal packets destined for the server.

        case PacketID::Login:
            handleLogin(packet->as<Protocol::Packet::Login>());
            handled = true;
            break;
        case PacketID::ClientToServerHandshake:
            break;
        case PacketID::ClientCacheStatus:
            break;
        case PacketID::ResourcePackClientResponse:
            break;
        case PacketID::ResourcePackChunkRequest:
            break;
        case PacketID::RequestChunkRadius:
            break;
        case PacketID::SetLocalPlayerAsInitialised:
            break;

            // Internal packets destined for the client.

        case PacketID::ServerToClientHandshake:
            handleServerToClientHandshake(packet->as<Protocol::Packet::ServerToClientHandshake>());
            handled = true;
            break;
        case PacketID::PlayStatus:
            handlePlayStatus(packet->as<PlayStatus>());
            handled = true;
            break;
        case PacketID::ResourcePacksInfo:
            handleResourcePacksInfo(packet->as<Protocol::Packet::ResourcePacksInfo>());
            handled = true;
            break;
        case PacketID::ResourcePackDataInfo:
            break;
        case PacketID::ResourcePackChunkData:
            break;
        case PacketID::ResourcePackStack:
            handleResourcePackStack(packet->as<Protocol::Packet::ResourcePackStack>());
            handled = true;
            break;
        case PacketID::StartGame:
            break;
        case PacketID::ChunkRadiusUpdated:
            break;
        default:
            // Should not happen;
            fmt::print("Unexpected packet ID in internal handler: {}", packet->Id());
    }
    if (!handled) {
        fmt::print("Received packet ID {}\n", packet->Id());
    }
    Protocol::Packet::PacketPool::Pool.Return(packet);
    flush();
}

void Client::handleLogin(Protocol::Packet::Login packet) {
    fmt::print("Received login packet protocol {}.", packet.ClientProtocol);
    throw Exception("TODO: Implement Login Handler");
    // https://github.com/Sandertv/gophertunnel/blob/master/minecraft/conn.go#L612
}

// handleServerToClientHandshake handles an incoming ServerToClientHandshake packet. It initialises encryption
// on the client side of the connection, using the hash and the public key from the server exposed in the
// packet.
void Client::handleServerToClientHandshake(Protocol::Packet::ServerToClientHandshake packet) {
    std::string jwt(packet.JWT.begin(), packet.JWT.end());
    // fmt::print("Received ServerToClientHandshake with JWT={}.", jwt);
    auto handshake = jwt::decode(jwt);
    auto key = handshake.get_header_claims()["x5u"].as_string();
    auto pub = Crypto::KeyPair(Base64::Decode(key));

    // Validate Claims
    try {
        jwt::verify()
            .allow_algorithm(jwt::algorithm::es384{pub.PublicToPEM()})
            .verify(handshake);
    } catch (jwt::signature_verification_exception &e) {
        throw Exception(fmt::format("error validating signature: {}", e.what()));
    }

    auto salt = Base64::Decode(handshake.get_payload_claim("salt").as_string());
    auto sharedSecret = keyPair.DHComputeKey(pub);
    auto ekey = ByteBuffer();
    ekey.resize(32);

    SHA256_CTX hasher;
    SHA256_Init(&hasher);
    SHA256_Update(&hasher, salt.data(), salt.size());
    SHA256_Update(&hasher, sharedSecret.data(), sharedSecret.size());
    SHA256_Final(ekey.data(), &hasher);

    encoder->EnableEncryption(ekey);
    decoder->EnableEncryption(ekey);

    this->WritePacket(ClientToServerHandshake());
}

void Client::handlePlayStatus(Protocol::Packet::PlayStatus packet) {
    SetLocalPlayerAsInitialised localPlayerInitialized;

    fmt::print("Received PlayStatus with Code={}.\n", packet.Status);
    switch (packet.Status) {
        case PlayStatusCode::LoginSuccess:
            WritePacket(Protocol::Packet::ClientCacheStatus());  // Maybe true?
            expect({PacketID::ResourcePacksInfo});
            break;
        case PlayStatusCode::LoginFailedClient:
            Disconnect();
            fmt::print("client outdated");
            break;
        case PlayStatusCode::LoginFailedServer:
            Disconnect();
            fmt::print("server outdated");
            break;
        case PlayStatusCode::PlayerSpawn:
            fmt::print("Player spawned");
            WritePacket(localPlayerInitialized);  // Maybe custom?
            break;
        case PlayStatusCode::LoginFailedInvalidTenant:
            Disconnect();
            fmt::print("invalid edu edition game owner");
            break;
        case PlayStatusCode::LoginFailedEduVanilla:
            Disconnect();
            fmt::print("cannot join an edu edition game on vanilla");
            break;
        case PlayStatusCode::LoginFailedServerFull:
            Disconnect();
            fmt::print("server full");
            break;
        default:
            fmt::print("unknown playstatus code: {}", packet.Status);
    }
}

void Client::handleResourcePacksInfo(Protocol::Packet::ResourcePacksInfo packet) {
    // TODO: Download packs
    expect({PacketID::ResourcePackStack});
    // fmt::print("Texture Packs to download: \n");
    // for(auto &c: packet.TexturePacks) {
    //     fmt::print("\t{} - {}\n", c.UUID, c.Version);
    // }
    // fmt::print("Behaviour Packs to download: \n");
    // for(auto &c: packet.BehaviourPacks) {
    //     fmt::print("\t{} - {}\n", c.UUID, c.Version);
    // }
    WritePacket(ResourcePackClientResponse(PackResponseCode::AllPacksDownloaded));
}

void Client::handleResourcePackStack(Protocol::Packet::ResourcePackStack packet) {
    expect({PacketID::StartGame});
    WritePacket(ResourcePackClientResponse(PackResponseCode::Completed));
}