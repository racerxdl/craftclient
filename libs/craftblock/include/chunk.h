#pragma once

#include <vector>
#include "block.h"

namespace CraftBlock {
    // Sizes for 1.18+
    constexpr int CHUNK_SIZE_X = 16;
    constexpr int CHUNK_SIZE_Z = 16;
    constexpr int CHUNK_SIZE_Y = 384;
    constexpr int CHUNK_SIZE_Y_OFF = -64; // Subtracted from Y. 1.18+ Y starts at -64

    struct Chunk {
    protected:
        std::vector<UBlock> blocks;
        static int BlockOffset(int x, int z, int y);

    public:
        Chunk();
        virtual void PutBlock(int x, int z, int y, UBlock block);
        void PutBlock(int x, int z, int y, CraftBlock::BlockType type);
        UBlock GetBlock(int x, int z, int y) const;
        std::string BlockName(int x, int z, int y) const;
        CraftBlock::BlockType BlockType(int x, int z, int y) const;
        bool BlockIsTransparent(int x, int z, int y) const;
    };
}