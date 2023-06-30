#include "protocol/nbt/tag.h"

#include <fmt/format.h>

using namespace ProtoRock::Protocol::NBT;

ProtoRock::Protocol::NBT::ITag ProtoRock::Protocol::NBT::createTag(const TagType type) {
    switch (type) {
        case TagType::End:
            return std::make_shared<TagEnd>();
        case TagType::Byte:
            return std::make_shared<TagByte>();
        case TagType::Int16:
            return std::make_shared<TagInt16>();
        case TagType::Int32:
            return std::make_shared<TagInt32>();
        case TagType::Int64:
            return std::make_shared<TagInt64>();
        case TagType::Float:
            return std::make_shared<TagFloat>();
        case TagType::Double:
            return std::make_shared<TagDouble>();
        case TagType::ByteArray:
            return std::make_shared<TagByteArray>();
        case TagType::String:
            return std::make_shared<TagString>();
        case TagType::List:
            return std::make_shared<TagList>();
        // case TagType::Struct: // TODO
        //     return std::make_shared<TagStruct>();
        case TagType::Int32Array:
            return std::make_shared<TagInt32Array>();
        case TagType::Int64Array:
            return std::make_shared<TagInt64Array>();
    }
    throw Common::Exception(fmt::format("unknown tag type {}", type));
}