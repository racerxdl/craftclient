#include "common/compressor.h"
#include "common/objectpool.h"

using namespace ProtoRock;
using namespace ProtoRock::Common;

Compressor::Compressor() {
    compressor = libdeflate_alloc_compressor(6);
}

Compressor::~Compressor() {
    libdeflate_free_compressor(compressor);
}

std::shared_ptr<ByteBuffer> Compressor::Compress(std::shared_ptr<ByteBuffer> in) {
    auto out = ObjectPools::ByteBufferPool.Get();
    auto maxSize = libdeflate_deflate_compress_bound(compressor, in->size());
    out->resize(maxSize);
    auto finalSize = libdeflate_deflate_compress(compressor, in->data(), in->size(), out->data(), out->size());
    out->resize(finalSize);
    return out;
}

ObjectPool<Compressor> ObjectPools::CompressorPool;