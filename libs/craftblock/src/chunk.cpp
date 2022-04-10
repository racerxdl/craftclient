#include "craftblock.h"

#include <cassert>
#include <map>

namespace CraftBlock {
    Chunk::Chunk() {
        AirBlock block;
        blocks.resize(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z); // Pre-allocate all chunks
        for (int i = 0; i < blocks.size(); i++) {
            blocks[i] = block.ToUBlock(); // Fill with airblocks
        }
    }

    int Chunk::BlockOffset(int x, int z, int y) {
        y -= CHUNK_SIZE_Y_OFF;

        assert(x < CHUNK_SIZE_X);
        assert(y < CHUNK_SIZE_Y);
        assert(z < CHUNK_SIZE_Z);
        assert(x >= 0);
        assert(y >= 0);
        assert(z >= 0);

        int p = x;
        p += z * CHUNK_SIZE_X;
        p += y * CHUNK_SIZE_Z * CHUNK_SIZE_X;

        return p;
    }

    void Chunk::PutBlock(int x, int z, int y, CraftBlock::BlockType type) {
        assert(BlockByType.count(type) > 0);
        PutBlock(x,z,y, BlockByType.at(type)->ToUBlock());
    }

    void Chunk::PutBlock(int x, int z, int y, UBlock block) {
        int p = Chunk::BlockOffset(x,z,y);
        blocks[p] = block->ToUBlock(); // clone
    }

    UBlock Chunk::GetBlock(int x, int z, int y) const {
        int p = Chunk::BlockOffset(x,z,y);
        return blocks[p]->ToUBlock(); // clone
    }

    std::string Chunk::BlockName(int x, int z, int y) const {
        int p = Chunk::BlockOffset(x,z,y);
        return blocks[p]->Name();
    }

    CraftBlock::BlockType Chunk::BlockType(int x, int z, int y) const {
        int p = Chunk::BlockOffset(x,z,y);
        return blocks[p]->Type();
    }
    bool Chunk::BlockIsTransparent(int x, int z, int y) const {
        int p = Chunk::BlockOffset(x,z,y);
        return blocks[p]->IsTransparent();
    }
}