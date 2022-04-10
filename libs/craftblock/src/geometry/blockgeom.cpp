#include "geometry/blockgeom.h"
#include "geometry/meshtools.h"
#include "blocks/blocks.h"

namespace CraftBlock {
    auto blockGeometry = BlockGeometry{
        GenerateCubeFaces(1, {0,0,0})
    };

    auto liquidGeometry = BlockGeometry{
        GenerateLiquidCubeFaces(1, {0,0,0})
    };

    auto xspriteGeometry = BlockGeometry{
        GenerateXSprite(1, {0,0,0})
    };

    auto torchGeometry = BlockGeometry{
        GenerateTorchFaces(1, {0,0,0})
    };

    std::unordered_map<CraftBlock::BlockType, BlockGeometry> BlockGeometries = {
        {CraftBlock::Air, blockGeometry},
        {CraftBlock::Grass, blockGeometry},
        {CraftBlock::Dirt, blockGeometry},
        {CraftBlock::Stone, blockGeometry},
        {CraftBlock::Sand, blockGeometry},
        {CraftBlock::Water, liquidGeometry},
        {CraftBlock::Log, blockGeometry},
        {CraftBlock::Leaves, blockGeometry},
        {CraftBlock::Rose, xspriteGeometry},
        {CraftBlock::Buttercup, blockGeometry},
        {CraftBlock::Coal, blockGeometry},
        {CraftBlock::Copper, blockGeometry},
        {CraftBlock::Lava, liquidGeometry},
        {CraftBlock::Clay, blockGeometry},
        {CraftBlock::Gravel, blockGeometry},
        {CraftBlock::Planks, blockGeometry},
        {CraftBlock::Torch, torchGeometry},
        {CraftBlock::Cobblestone, blockGeometry},
        {CraftBlock::Snow, blockGeometry},
        {CraftBlock::Podzol, blockGeometry},
        {CraftBlock::Shrub, blockGeometry},
        {CraftBlock::TallGrass, xspriteGeometry},
        {CraftBlock::PineLog, blockGeometry},
        {CraftBlock::PineLeaves, blockGeometry},
        {CraftBlock::Brick, blockGeometry},
        {CraftBlock::Cactus, blockGeometry},
    };
}