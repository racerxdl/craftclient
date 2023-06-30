#pragma once

#include <string>

#include "protocol/packet/reader.h"
#include "protocol/packet/writer.h"

// See https://github.com/Sandertv/gophertunnel/blob/master/minecraft/protocol/experiments.go
namespace ProtoRock {
namespace Protocol {

// ExperimentData holds data on an experiment that is either enabled or disabled.
struct ExperimentData {
    // Name is the name of the experiment.
    std::string Name;
    // Enabled specifies if the experiment is enabled. Vanilla typically always sets this to true for any
    // experiments sent.
    bool Enabled = false;

    void Serialize(Packet::Writer &w) const {
        w.Write(Name);
        w.Write(Enabled);
    };
    void Deserialize(Packet::Reader &r) {
        r.Read(Name);
        r.Read(Enabled);
    }
};

}  // namespace Protocol
}  // namespace ProtoRock