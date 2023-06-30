#pragma once

#include <memory>
#include <string>
#include <vector>

#include "common/common.h"
#include "protocol/packet/reader.h"
#include "protocol/packet/writer.h"

namespace ProtoRock {
namespace Protocol {
namespace NBT {

// IT IS NETWORK LITTLE ENDIAN (why mojang)
// This means integers are varint

enum struct TagType : uint8_t {
    End = 0,
    Byte,
    Int16,
    Int32,
    Int64,
    Float,
    Double,
    ByteArray,
    String,
    List,
    Struct,
    Int32Array,
    Int64Array,
};

struct Tag {
    std::string name;

    std::string Name() const { return name; };
    void SetName(const std::string &n) { name = n; };

    virtual TagType Type() const = 0;
    virtual void Serialize(Packet::Writer &w) const = 0;
    virtual void Deserialize(Packet::Reader &r) = 0;
};

using ITag = std::shared_ptr<Tag>;

ITag createTag(const TagType type);

struct TagEnd : public Tag {
    TagType Type() const { return TagType::End; };
    void Serialize(Packet::Writer &w) const {};
    void Deserialize(Packet::Reader &r){};
};

struct TagByte : public Tag {
    uint8_t value;
    TagType Type() const { return TagType::Byte; };
    void Serialize(Packet::Writer &w) const {
        w.Write(value);
    };
    void Deserialize(Packet::Reader &r) {
        r.Read(value);
    };
};

struct TagInt16 : public Tag {
    uint16_t value;
    TagType Type() const { return TagType::Int16; };
    void Serialize(Packet::Writer &w) const {
        w.WriteLE(value);
    };
    void Deserialize(Packet::Reader &r) {
        r.ReadLE(value);
    };
};

struct TagInt32 : public Tag {
    int32_t value;
    TagType Type() const { return TagType::Int32; };
    void Serialize(Packet::Writer &w) const {
        w.WriteVarint<int32_t>(value);
    };
    void Deserialize(Packet::Reader &r) {
        r.ReadVarint<int32_t>(value);
    };
};

struct TagInt64 : public Tag {
    int64_t value;
    TagType Type() const { return TagType::Int64; };
    void Serialize(Packet::Writer &w) const {
        w.WriteVarint<int64_t>(value);
    };
    void Deserialize(Packet::Reader &r) {
        r.ReadVarint<int64_t>(value);
    };
};

struct TagFloat : public Tag {
    float value;
    TagType Type() const { return TagType::Float; };
    void Serialize(Packet::Writer &w) const {
        w.WriteFloat(value);
    };
    void Deserialize(Packet::Reader &r) {
        r.ReadFloat(value);
    };
};

struct TagDouble : public Tag {
    double value;
    TagType Type() const { return TagType::Double; };
    std::string Name() const { return name; }
    void Serialize(Packet::Writer &w) const {
        w.WriteDouble(value);
    };
    void Deserialize(Packet::Reader &r) {
        r.ReadDouble(value);
    };
};

struct TagByteArray : public Tag {
    Common::ByteBuffer value;
    TagType Type() const { return TagType::ByteArray; };
    void Serialize(Packet::Writer &w) const {
        w.Write(value);
    };
    void Deserialize(Packet::Reader &r) {
        r.Read(value);
    };
};

struct TagString : public Tag {
    std::string value;
    TagType Type() const { return TagType::String; };
    std::string Name() const { return name; }
    void Serialize(Packet::Writer &w) const {
        w.Write(value);
    };
    void Deserialize(Packet::Reader &r) {
        r.Read(value);
    };
};

struct TagList : public Tag {
    std::vector<ITag> value;
    TagType contentType;
    TagType Type() const { return TagType::List; };
    TagType ContentType() const { return contentType; }

    void Serialize(Packet::Writer &w) const {
        w.Write(static_cast<uint8_t>(contentType));
        w.WriteVarint<uint32_t>(value.size());
        for (auto &tag : value) {
            tag->Serialize(w);
        }
    };
    void Deserialize(Packet::Reader &r) {
        r.Read((uint8_t &)(contentType));
        uint32_t size;
        r.ReadVarint<uint32_t>(size);
        for (int i = 0; i < size; i++) {
            auto tag = createTag(contentType);
            tag->Deserialize(r);
            value.push_back(tag);
        }
    };
};

struct TagInt32Array : public Tag {
    std::vector<int32_t> value;
    TagType Type() const { return TagType::Int32Array; };
    void Serialize(Packet::Writer &w) const {
        w.WriteVarint<uint32_t>(value.size());
        for (auto v : value) {
            w.WriteVarint<int32_t>(v);
        }
    };
    void Deserialize(Packet::Reader &r) {
        uint32_t size;
        r.ReadVarint<uint32_t>(size);
        for (int i = 0; i < size; i++) {
            int32_t v;
            r.ReadVarint<int32_t>(v);
            value.push_back(v);
        }
    };
};

struct TagInt64Array : public Tag {
    std::vector<int64_t> value;
    TagType Type() const { return TagType::Int64Array; };
    void Serialize(Packet::Writer &w) const {
        w.WriteVarint<uint32_t>(value.size());
        for (auto v : value) {
            w.WriteVarint<int64_t>(v);
        }
    };
    void Deserialize(Packet::Reader &r) {
        uint32_t size;
        r.ReadVarint<uint32_t>(size);
        for (int i = 0; i < size; i++) {
            int64_t v;
            r.ReadVarint<int64_t>(v);
            value.push_back(v);
        }
    };
};

}  // namespace NBT
}  // namespace Protocol
}  // namespace ProtoRock