#pragma once

#include <cstdint>

#include "common.h"

namespace ProtoRock {
constexpr uint8_t IDConnectedPing = 0x00;
constexpr uint8_t IDUnconnectedPing = 0x01;
constexpr uint8_t IDUnconnectedPingOpenConnections = 0x02;
constexpr uint8_t IDConnectedPong = 0x03;
constexpr uint8_t IDDetectLostConnections = 0x04;
constexpr uint8_t IDOpenConnectionRequest1 = 0x05;
constexpr uint8_t IDOpenConnectionReply1 = 0x06;
constexpr uint8_t IDOpenConnectionRequest2 = 0x07;
constexpr uint8_t IDOpenConnectionReply2 = 0x08;
constexpr uint8_t IDConnectionRequest = 0x09;
constexpr uint8_t IDConnectionRequestAccepted = 0x10;
constexpr uint8_t IDNewIncomingConnection = 0x13;
constexpr uint8_t IDDisconnectNotification = 0x15;
constexpr uint8_t IDIncompatibleProtocolVersion = 0x19;
constexpr uint8_t IDUnconnectedPong = 0x1c;
constexpr uint8_t IDGamePacket = 0xfe;

static uint8_t unconnectedMessageSequence[16] = {0x00, 0xff, 0xff, 0x00, 0xfe, 0xfe, 0xfe, 0xfe, 0xfd, 0xfd, 0xfd, 0xfd, 0x12, 0x34, 0x56, 0x78};


struct UnconnectedPing {
    uint8_t magic[16];
    uint64_t sendTimestamp = 0;
    uint64_t clientGUID = 0;

    void Write(PacketBuff &b) {
        b.Write(IDUnconnectedPing);
        b.WriteBE(sendTimestamp);
        for (int i = 0; i < 16; i++) {
            b.Write(unconnectedMessageSequence[i]);
        }
        b.WriteBE(clientGUID);
    }
    void Read(PacketBuff &b) {
        sendTimestamp = b.ReadU64BE();
        for (int i = 0; i < 16; i++) {
            magic[i] = b.Read();
        }
        clientGUID = b.ReadU64BE();
    }
};

struct UnconnectedPong {
    uint8_t magic[16];
    uint64_t sendTimestamp = 0;
    uint64_t serverGUID = 0;
    ByteBuffer data;

    void Write(PacketBuff &b) {
        b.Write(IDUnconnectedPong);
        b.WriteBE(sendTimestamp);
        b.WriteBE(serverGUID);
        for (int i = 0; i < 16; i++) {
            b.Write(unconnectedMessageSequence[i]);
        }
        b.WriteBE((int16_t)data.size());
        for (int i = 0; i < data.size(); i++) {
            b.Write(data[i]);
        }
    }

    void Read(PacketBuff &b) {
        sendTimestamp = b.ReadU64BE();
        serverGUID = b.ReadU64BE();
        for (int i = 0; i < 16; i++) {
            magic[i] = b.Read();
        }
        auto size = b.ReadU16BE();
        data.resize(size);
        for (int i = 0; i < size; i++) {
            data[i] = b.Read();
        }
    }
};

struct OpenConnectionRequest1 {
    uint8_t magic[16];
    uint8_t protocol = 0;
    uint16_t maximumSizeNotDropped = 0;

    void Write(PacketBuff &b) {
        b.Write(IDOpenConnectionRequest1);
        for (int i = 0; i < 16; i++) {
            b.Write(unconnectedMessageSequence[i]);
        }
        b.Write(protocol);
        b.WriteBE((uint16_t)(maximumSizeNotDropped - b.size() + 28));
    }
    void Read(PacketBuff &b) {
        maximumSizeNotDropped = (b.size() + 1) + 28;
        for (int i = 0; i < 16; i++) {
            magic[i] = b.Read();
        }
        protocol = b.Read();
    }
};

struct IncompatibleProtocolVersion {
    uint8_t magic[16];
    uint8_t serverProtocol = 0;
    uint64_t serverGUID = 0;

    void Write(PacketBuff &b) {
        b.Write(IDOpenConnectionRequest1);
        for (int i = 0; i < 16; i++) {
            b.Write(unconnectedMessageSequence[i]);
        }
        b.Write(serverProtocol);
        b.WriteBE(serverGUID);
    }
    void Read(PacketBuff &b) {
        for (int i = 0; i < 16; i++) {
            magic[i] = b.Read();
        }
        serverProtocol = b.Read();
        serverGUID = b.ReadU64BE();
    }
};

struct OpenConnectionRequest2 {
    uint8_t magic[16];
    asio::ip::udp::endpoint serverAddress;
    uint16_t clientPreferredMTUSize = 0;
    uint64_t clientGUID = 0;

    void Write(PacketBuff &b) {
        b.Write(IDOpenConnectionRequest2);
        for (int i = 0; i < 16; i++) {
            b.Write(unconnectedMessageSequence[i]);
        }
        b.Write(serverAddress);
        b.WriteBE(clientPreferredMTUSize);
        b.WriteBE(clientGUID);
    }
    void Read(PacketBuff &b) {
        for (int i = 0; i < 16; i++) {
            magic[i] = b.Read();
        }
        serverAddress = b.ReadAddress();
        clientPreferredMTUSize = b.ReadU16BE();
        clientGUID = b.ReadU64BE();
    }
};

struct OpenConnectionReply1 {
    uint8_t magic[16];
    uint64_t serverGUID = 0;
    bool secure = false;
    uint16_t serverPreferredMTUSize = 0;

    void Write(PacketBuff &b) {
        b.Write(IDOpenConnectionReply1);
        for (int i = 0; i < 16; i++) {
            b.Write(unconnectedMessageSequence[i]);
        }
        b.WriteBE(serverGUID);
        b.Write((uint8_t)(secure ? 1 : 0));
        b.WriteBE(serverPreferredMTUSize);
    }
    void Read(PacketBuff &b) {
        for (int i = 0; i < 16; i++) {
            magic[i] = b.Read();
        }
        serverGUID = b.ReadU64BE();
        secure = b.Read() == 1;
        serverPreferredMTUSize = b.ReadU16BE();
    }
};

struct OpenConnectionReply2 {
    uint8_t magic[16];
    uint64_t serverGUID = 0;
    asio::ip::udp::endpoint clientAddress;
    uint16_t mtuSize = 0;
    bool secure = false;

    void Write(PacketBuff &b) {
        b.Write(IDOpenConnectionReply2);
        for (int i = 0; i < 16; i++) {
            b.Write(unconnectedMessageSequence[i]);
        }
        b.WriteBE(serverGUID);
        b.Write(clientAddress);
        b.WriteBE(mtuSize);
        b.Write((uint8_t)(secure ? 1 : 0));
    }
    void Read(PacketBuff &b) {
        for (int i = 0; i < 16; i++) {
            magic[i] = b.Read();
        }
        serverGUID = b.ReadU64BE();
        clientAddress = b.ReadAddress();
        mtuSize = b.ReadU16BE();
        secure = b.Read() == 1;
    }
};

struct ConnectedPing {
    int64_t ClientTimestamp = 0;
    void Write(PacketBuff &b) {
        b.Write(IDConnectedPing);
        b.WriteBE(ClientTimestamp);
    }
    void Read(PacketBuff &b) { ClientTimestamp = b.ReadI64BE(); }
};

struct ConnectedPong {
    int64_t ClientTimestamp = 0;
    int64_t ServerTimestamp = 0;

    void Write(PacketBuff &b) {
        b.Write(IDConnectedPong);
        b.WriteBE(ClientTimestamp);
        b.WriteBE(ServerTimestamp);
    }
    void Read(PacketBuff &b) {
        ClientTimestamp = b.ReadI64BE();
        ServerTimestamp = b.ReadI64BE();
    }
};

struct ConnectionRequest {
    int64_t ClientGUID = 0;
    int64_t RequestTimestamp = 0;
    bool Secure = false;

    void Write(PacketBuff &b) {
        b.Write(IDConnectionRequest);
        b.WriteBE(ClientGUID);
        b.WriteBE(RequestTimestamp);
        b.Write((uint8_t)(Secure ? 1 : 0));
    }
    void Read(PacketBuff &b) {
        ClientGUID = b.ReadI64BE();
        RequestTimestamp = b.ReadI64BE();
        Secure = b.Read() == 1;
    }
};

struct ConnectionRequestAccepted {
    asio::ip::udp::endpoint clientAddress;
    asio::ip::udp::endpoint systemAddresses[20];
    int64_t RequestTimestamp = 0;
    int64_t AcceptedTimestamp = 0;

    ConnectionRequestAccepted() {
        clientAddress = asio::ip::udp::endpoint();
        for (int i = 0; i < 20; i++) {
            systemAddresses[i] = asio::ip::udp::endpoint();
        }
    }

    void Write(PacketBuff &b) {
        b.Write(IDConnectionRequestAccepted);
        b.Write(clientAddress);
        b.WriteBE((int16_t)0);
        for (int i = 0; i < 20; i++) {
            b.Write(systemAddresses[i]);
        }
        b.WriteBE(RequestTimestamp);
        b.WriteBE(AcceptedTimestamp);
    }
    void Read(PacketBuff &b) {
        clientAddress = b.ReadAddress();
        b.ReadI16BE();  // Discard
        for (int i = 0; i < 20; i++) {
            systemAddresses[i] = b.ReadAddress();
        }
        RequestTimestamp = b.ReadI64BE();
        AcceptedTimestamp = b.ReadI64BE();
    }
};

struct NewIncomingConnection {
    asio::ip::udp::endpoint serverAddress;
    asio::ip::udp::endpoint systemAddresses[20];
    int64_t RequestTimestamp = 0;
    int64_t AcceptedTimestamp = 0;

    NewIncomingConnection() {
        serverAddress = asio::ip::udp::endpoint();
        for (int i = 0; i < 20; i++) {
            systemAddresses[i] = asio::ip::udp::endpoint();
        }
    }

    void Write(PacketBuff &b) {
        b.Write(IDNewIncomingConnection);
        b.Write(serverAddress);
        for (int i = 0; i < 20; i++) {
            b.Write(systemAddresses[i]);
        }
        b.WriteBE(RequestTimestamp);
        b.WriteBE(AcceptedTimestamp);
    }
    void Read(PacketBuff &b) {
        serverAddress = b.ReadAddress();
        for (int i = 0; i < 20; i++) {
            systemAddresses[i] = b.ReadAddress();
            if (b.size() == 16) {
                break;
            }
        }
        RequestTimestamp = b.ReadI64BE();
        AcceptedTimestamp = b.ReadI64BE();
    }
};

}  // namespace ProtoRock