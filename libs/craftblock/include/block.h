#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CraftBlock {
    struct Block;
    typedef std::vector<std::string> BlockFaceTextureNames;
    typedef std::unique_ptr<Block> UBlock;

    enum BlockType {
        Air = 0,
        Grass = 1,
        Dirt = 2,
        Stone = 3,
        Sand = 4,
        Water = 5,
        Glass = 6,
        Log = 7,
        Leaves = 8,
        Rose = 9,
        Buttercup = 10,
        Coal = 11,
        Copper = 12,
        Lava = 13,
        Clay = 14,
        Gravel = 15,
        Planks = 16,
        Torch = 17,
        Cobblestone = 18,
        Snow = 19,
        Podzol = 20,
        Shrub = 21,
        TallGrass = 22,
        PineLog = 23,
        PineLeaves = 24,
        Brick = 25,
        Cactus = 26,
    };

    struct Block {
        virtual const std::string Name() const = 0;
        virtual const BlockFaceTextureNames TextureNames() const { return BlockFaceTextureNames(); };
        virtual UBlock ToUBlock() const = 0;
        virtual BlockType Type() const { return BlockType::Air; };
        virtual bool IsTransparent() const { return true; };
    };
}