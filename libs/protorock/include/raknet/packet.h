#pragma once

#include <exception>
#include <memory>
#include <string>

#include "common/common.h"

namespace ProtoRock {

using namespace ProtoRock::Common;

// Based / Ported from https://github.com/Sandertv/go-raknet/blob/master/packet.go

// bitFlagDatagram is set for every valid datagram. It is used to identify packets that are datagrams.
constexpr int bitFlagDatagram = 0x80;
// bitFlagACK is set for every ACK packet.
constexpr int bitFlagACK = 0x40;
// bitFlagNACK is set for every NACK packet.
constexpr int bitFlagNACK = 0x20;

// reliabilityUnreliable means that the packet sent could arrive out of order, be duplicated, or just not
// arrive at all. It is usually used for high frequency packets of which the order does not matter.
// lint:ignore U1000 While this constant is unused, it is here for the sake of having all reliabilities.
constexpr uint8_t reliabilityUnreliable = 0;
// reliabilityUnreliableSequenced means that the packet sent could be duplicated or not arrive at all, but
// ensures that it is always handled in the right order.
constexpr uint8_t reliabilityUnreliableSequenced = 1;
// reliabilityReliable means that the packet sent could not arrive, or arrive out of order, but ensures
// that the packet is not duplicated.
constexpr uint8_t reliabilityReliable = 2;
// reliabilityReliableOrdered means that every packet sent arrives, arrives in the right order and is not
// duplicated.
constexpr uint8_t reliabilityReliableOrdered = 3;
// reliabilityReliableSequenced means that the packet sent could not arrive, but ensures that the packet
// will be in the right order and not be duplicated.
constexpr uint8_t reliabilityReliableSequenced = 4;
// splitFlag is set in the header if the packet was split. If so, the encapsulation contains additional
// data about the fragment.
constexpr uint8_t splitFlag = 0x10;

// packetRange indicates a range of packets, followed by the first and the last packet in the range.
constexpr uint8_t packetRange = 0;
// packetSingle indicates a single packet, followed by its sequence number.
constexpr uint8_t packetSingle = 1;

struct packet {
    uint8_t reliability;
    PacketBuff content;
    uint24_t messageIndex;
    uint24_t sequenceIndex;
    uint24_t orderIndex;

    bool split;
    uint32_t splitCount;
    uint32_t splitIndex;
    uint16_t splitId;

    void write(PacketBuff &buff);
    // Might throw exception if not enough bytes
    void read(PacketBuff &buff);
    bool reliable();
    bool sequencedOrOrdered();
    bool sequenced();
};

struct acknowledgement {
    std::vector<uint24_t> packets;
    int write(PacketBuff &buff, int16_t mtu);
    void read(PacketBuff &buff);
};

class PacketReadException : public std::exception {
   public:
    std::string msg;
    PacketReadException(const std::string &msg) : msg(msg) {}
    virtual const char *what() const throw() {
        return msg.c_str();
    }
};

typedef std::shared_ptr<packet> Packet;

inline Packet MakePacket() { return std::make_shared<packet>(); }

}  // namespace ProtoRock