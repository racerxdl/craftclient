#pragma once

#include <craftblock.h>
#include "object.h"

struct ChunkObject: public Object, public CraftBlock::Chunk {
private:
    bool needsUpdate = true;
    int BlockFaceOffset(int x, int z, int y) { return BlockOffset(x,z,y) * 6; }
public:
    ChunkObject();
    ChunkObject(TextureAtlas atlas) : ChunkObject() { this->atlas = atlas; this->tex = atlas; }
    ChunkObject(Texture tex) : ChunkObject() { this->tex = tex; }
    void Update() override;
    void PutBlock(int x, int z, int y, CraftBlock::UBlock block) override;
    void PutBlock(int x, int z, int y, CraftBlock::BlockType type);
};

