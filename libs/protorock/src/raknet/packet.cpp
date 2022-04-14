#include "raknet/packet.h"

#include <endian.h>

#include <algorithm>

namespace ProtoRock {
void packet::write(PacketBuff &buff) {
    auto header = reliability << 5;
    if (split) {
        header |= splitFlag;
    }
    buff.push_back(header);
    buff.WriteBE((uint16_t)(content.size() << 3));
    if (reliable()) {
        buff.WriteUint24(messageIndex);
    }
    if (sequenced()) {
        buff.WriteUint24(sequenceIndex);
    }
    if (sequencedOrOrdered()) {
        buff.WriteUint24(orderIndex);
        buff.Write(0);
    }
    if (split) {
        buff.WriteBE(splitCount);
        buff.WriteBE(splitId);
        buff.WriteBE(splitIndex);
    }
    buff.Write(content);
}

void packet::read(PacketBuff &buff) {
    auto header = buff.Read();
    split = (header & splitFlag) != 0;
    reliability = (header & 224) >> 5;
    auto packetLength = buff.ReadU16BE();
    packetLength = packetLength >> 3;
    if (packetLength == 0) {
        throw PacketReadException("packet length cannot be 0");
    }
    if (reliable()) {
        messageIndex = buff.ReadUint24();
    }
    if (sequenced()) {
        sequenceIndex = buff.ReadUint24();
    }
    if (sequencedOrOrdered()) {
        orderIndex = buff.ReadUint24();
        buff.Read();  // Discard one byte
    }
    if (split) {
        splitCount = buff.ReadU32BE();
        splitId = buff.ReadU16BE();
        splitIndex = buff.ReadU32BE();
    }
    content = PacketBuff(buff.ReadBytes(packetLength));
}

bool packet::reliable() {
    switch (reliability) {
        case reliabilityReliable:
        case reliabilityReliableOrdered:
        case reliabilityReliableSequenced:
            return true;
        default:
            return false;
    }
}
bool packet::sequencedOrOrdered() {
    switch (reliability) {
        case reliabilityUnreliableSequenced:
        case reliabilityReliableOrdered:
        case reliabilityReliableSequenced:
            return true;
        default:
            return false;
    }
}

bool packet::sequenced() {
    switch (reliability) {
        case reliabilityUnreliableSequenced:
        case reliabilityReliableSequenced:
            return true;
        default:
            return false;
    }
}

int acknowledgement::write(PacketBuff &b, int16_t mtu) {
    if (packets.size() == 0) {
        b.WriteBE((int16_t)0);
        return 0;
    }
    std::sort(packets.begin(), packets.end(), [] (auto i, auto j) { return i < j; });

    uint24_t firstPacketInRange = 0;
    uint24_t lastPacketInRange = 0;
    int16_t recordCount = 0;

    auto buffer = PacketBuff();
    int n = 0;
    for (int i = 0; i < packets.size(); i++) {
        const auto &packet = packets[i];
        if (buffer.size() >= (mtu - 10)) {
            break;
        }
        n++;
        if (i == 0) {
            firstPacketInRange = packet;
            lastPacketInRange = packet;
            continue;
        }

        if (packet == lastPacketInRange + 1) {
            lastPacketInRange = packet;
            continue;
        } else {

            if (firstPacketInRange == lastPacketInRange) {
                buffer.Write(packetSingle);
                buffer.WriteUint24(firstPacketInRange);
            } else {
                buffer.Write(packetRange);
                buffer.WriteUint24(firstPacketInRange);
                buffer.WriteUint24(lastPacketInRange);
            }
            firstPacketInRange = packet;
            lastPacketInRange = packet;

            recordCount++;
        }
    }

    if (firstPacketInRange == lastPacketInRange) {
        buffer.Write(packetSingle);
        buffer.WriteUint24(firstPacketInRange);
    } else {
        buffer.Write(packetRange);
        buffer.WriteUint24(firstPacketInRange);
        buffer.WriteUint24(lastPacketInRange);
    }
    recordCount++;
    b.WriteBE(recordCount);
    b.Write(buffer);

    return n;
}

constexpr int maxAcknowledgementPackets = 8192;

void acknowledgement::read(PacketBuff &buff) {
    uint24_t start, end, packet;
    auto recordCount = buff.ReadI16BE();
    for (int i = 0; i < recordCount; i++) {
        auto recordType = buff.Read();

        switch (recordType) {
            case packetRange:
                start = buff.ReadUint24();
                end = buff.ReadUint24();
                for (int pack = start; pack <= end; pack++) {
                    packets.push_back(pack);
                    if (packets.size() > maxAcknowledgementPackets) {
                        throw PacketReadException("max amount of ack packets reached");
                    }
                }
                break;
            case packetSingle:
                packet = buff.ReadUint24();
                packets.push_back(packet);
                if (packets.size() > maxAcknowledgementPackets) {
                    throw PacketReadException("max amount of ack packets reached");
                }
                break;
        }
    }
}

}  // namespace ProtoRock