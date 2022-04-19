#pragma once

#include <libdeflate.h>

#include "common/common.h"

namespace ProtoRock {
namespace Common {

class Compressor {
   private:
    libdeflate_compressor *compressor;

   public:
    Compressor();
    ~Compressor();
    std::shared_ptr<ByteBuffer> Compress(std::shared_ptr<ByteBuffer> in);
};

}  // namespace Common
}  // namespace ProtoRock