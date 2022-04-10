#pragma once

#include <unordered_map>

#include "face.h"
#include "blocks/blocks.h"

namespace CraftBlock {
    struct BlockGeometry {
        FaceList faces;
    };

    extern std::unordered_map<BlockType, BlockGeometry> BlockGeometries;
}