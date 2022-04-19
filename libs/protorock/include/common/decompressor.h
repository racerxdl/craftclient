#pragma once

#include <libdeflate.h>

#include "common/common.h"

namespace ProtoRock {
namespace Common {

class Decompressor {
   private:
    libdeflate_decompressor *decompressor;

   public:
    Decompressor();
    ~Decompressor();
    std::shared_ptr<ByteBuffer> Decompress(std::shared_ptr<ByteBuffer> in);
};

}  // namespace Common
}  // namespace ProtoRock