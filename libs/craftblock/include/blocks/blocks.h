#pragma once

#include <memory>
#include <unordered_map>

#include "../block.h"
#include "builder.h"

namespace CraftBlock {
    BLOCKSTRUCT(Air, true)
    BLOCKSTRUCT(Grass, false)
    BLOCKSTRUCT(Dirt, false)
    BLOCKSTRUCT(Stone, false)
    BLOCKSTRUCT(Sand, false)
    BLOCKSTRUCT(Water, true)
    BLOCKSTRUCT(Glass, true)
    BLOCKSTRUCT(Log, false)
    BLOCKSTRUCT(Leaves, true)
    BLOCKSTRUCT(Rose, true)
    BLOCKSTRUCT(Buttercup, false)
    BLOCKSTRUCT(Coal, false)
    BLOCKSTRUCT(Copper, false)
    BLOCKSTRUCT(Lava, false)
    BLOCKSTRUCT(Clay, false)
    BLOCKSTRUCT(Gravel, false)
    BLOCKSTRUCT(Planks, false)
    BLOCKSTRUCT(Torch, true)
    BLOCKSTRUCT(Cobblestone, false)
    BLOCKSTRUCT(Snow, false)
    BLOCKSTRUCT(Podzol, false)
    BLOCKSTRUCT(Shrub, false)
    BLOCKSTRUCT(TallGrass, true)
    BLOCKSTRUCT(PineLog, false)
    BLOCKSTRUCT(PineLeaves, true)
    BLOCKSTRUCT(Brick, false)
    BLOCKSTRUCT(Cactus, false)

    extern const std::unordered_map<CraftBlock::BlockType, const Block*> BlockByType;
    BlockFaceTextureNames TexturesToLoad();
}