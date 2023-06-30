#pragma once

#include <string>

#include "protocol/packet/reader.h"
#include "protocol/packet/writer.h"

// See https://github.com/Sandertv/gophertunnel/blob/master/minecraft/protocol/resource_pack.go
namespace ProtoRock {
namespace Protocol {

// BehaviourPackInfo represents a behaviour pack's info sent over network. It holds information about the
// behaviour pack such as its name, description and version.
struct BehaviourPackInfo {
    // UUID is the UUID of the behaviour pack. Each behaviour pack downloaded must have a different UUID in
    // order for the client to be able to handle them properly.
    std::string UUID;
    // Version is the version of the behaviour pack. The client will cache behaviour packs sent by the server as
    // long as they carry the same version. Sending a behaviour pack with a different version than previously
    // will force the client to re-download it.
    std::string Version;
    // Size is the total size in bytes that the behaviour pack occupies. This is the size of the compressed
    // archive (zip) of the behaviour pack.
    uint64_t Size = 0;
    // ContentKey is the key used to decrypt the behaviour pack if it is encrypted. This is generally the case
    // for marketplace behaviour packs.
    std::string ContentKey;
    // SubPackName ...
    std::string SubPackName;
    // ContentIdentity ...
    std::string ContentIdentity;
    // HasScripts specifies if the behaviour packs has any scripts in it. A client will only download the
    // behaviour pack if it supports scripts, which, up to 1.11, only includes Windows 10.
    bool HasScripts = false;

    void Serialize(Packet::Writer &w) const {
        w.Write(UUID);
        w.Write(Version);
        w.WriteLE(Size);
        w.Write(ContentKey);
        w.Write(SubPackName);
        w.Write(ContentIdentity);
        w.Write(HasScripts);
    };
    void Deserialize(Packet::Reader &r) {
        r.Read(UUID);
        r.Read(Version);
        r.ReadLE(Size);
        r.Read(ContentKey);
        r.Read(SubPackName);
        r.Read(ContentIdentity);
        r.Read(HasScripts);
    }
};

// TexturePackInfo represents a texture pack's info sent over network. It holds information about the
// texture pack such as its name, description and version.
struct TexturePackInfo {
    // UUID is the UUID of the texture pack. Each texture pack downloaded must have a different UUID in
    // order for the client to be able to handle them properly.
    std::string UUID;
    // Version is the version of the texture pack. The client will cache texture packs sent by the server as
    // long as they carry the same version. Sending a texture pack with a different version than previously
    // will force the client to re-download it.
    std::string Version;
    // Size is the total size in bytes that the texture pack occupies. This is the size of the compressed
    // archive (zip) of the texture pack.
    uint64_t Size;
    // ContentKey is the key used to decrypt the behaviour pack if it is encrypted. This is generally the case
    // for marketplace texture packs.
    std::string ContentKey;
    // SubPackName ...
    std::string SubPackName;
    // ContentIdentity is another UUID for the resource pack, and is generally set for marketplace texture
    // packs. It is also required for client-side validations when the resource pack is encrypted.
    std::string ContentIdentity;
    // HasScripts specifies if the texture packs has any scripts in it. A client will only download the
    // behaviour pack if it supports scripts, which, up to 1.11, only includes Windows 10.
    bool HasScripts;
    // RTXEnabled specifies if the texture pack uses the raytracing technology introduced in 1.16.200.
    bool RTXEnabled;

    void Serialize(Packet::Writer &w) const {
        w.Write(UUID);
        w.Write(Version);
        w.WriteLE(Size);
        w.Write(ContentKey);
        w.Write(SubPackName);
        w.Write(ContentIdentity);
        w.Write(HasScripts);
        w.Write(RTXEnabled);
    };
    void Deserialize(Packet::Reader &r) {
        r.Read(UUID);
        r.Read(Version);
        r.ReadLE(Size);
        r.Read(ContentKey);
        r.Read(SubPackName);
        r.Read(ContentIdentity);
        r.Read(HasScripts);
        r.Read(RTXEnabled);
    }
};

// StackResourcePack represents a resource pack sent on the stack of the client. When sent, the client will
// apply them in the order of the stack sent.
struct StackResourcePack {
    // UUID is the UUID of the resource pack. Each resource pack downloaded must have a different UUID in
    // order for the client to be able to handle them properly.
    std::string UUID;
    // Version is the version of the resource pack. The client will cache resource packs sent by the server as
    // long as they carry the same version. Sending a resource pack with a different version than previously
    // will force the client to re-download it.
    std::string Version;
    // SubPackName ...
    std::string SubPackName;

    void Serialize(Packet::Writer &w) const {
        w.Write(UUID);
        w.Write(Version);
        w.Write(SubPackName);
    };
    void Deserialize(Packet::Reader &r) {
        r.Read(UUID);
        r.Read(Version);
        r.Read(SubPackName);
    }
};

}  // namespace Protocol
}  // namespace ProtoRock