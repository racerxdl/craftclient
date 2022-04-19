#include "common/decompressor.h"
#include "common/objectpool.h"
#include "fmt/format.h"

using namespace ProtoRock;
using namespace ProtoRock::Common;

Decompressor::Decompressor() {
    decompressor = libdeflate_alloc_decompressor();
}

Decompressor::~Decompressor() {
    libdeflate_free_decompressor(decompressor);
}

std::shared_ptr<ByteBuffer> Decompressor::Decompress(std::shared_ptr<ByteBuffer> in) {
    auto out = ObjectPools::ByteBufferPool.Get();
    out->resize(in->size()*2 + 1024); // We cannot estimate how big it will be. So let's assume 50% compression plus some bytes
    size_t actualBytesOut = 0;
    auto result = libdeflate_deflate_decompress(decompressor, in->data(), in->size(), out->data(), out->size(), &actualBytesOut);

    if (result == LIBDEFLATE_INSUFFICIENT_SPACE) {
        // Lets try again with a bigger buffer.
        out->resize(out->size()*2);
        result = libdeflate_deflate_decompress(decompressor, in->data(), in->size(), out->data(), out->size(), &actualBytesOut);
    }

    if (result == LIBDEFLATE_SUCCESS) {
        out->resize(actualBytesOut);
        return out;
    }

    throw Exception(fmt::format("cannot decompress payload. error {}", result));
}

ObjectPool<Decompressor> ObjectPools::DecompressorPool;