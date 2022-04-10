#include "mineblock.h"
#include "meshtools.h"

ChunkObject::ChunkObject() : Object(), Chunk() {
    FaceList cubeFaces = GenerateCubeFaces(1);
    FaceList chunkFaces = FaceList();

    chunkFaces.reserve(cubeFaces.size() * blocks.size());

    int p = 0;
    for(auto &block: blocks) {
        int x = p % CraftBlock::CHUNK_SIZE_X;
        int z = (p / CraftBlock::CHUNK_SIZE_X) % CraftBlock::CHUNK_SIZE_Z;
        int y = (p / (CraftBlock::CHUNK_SIZE_X*CraftBlock::CHUNK_SIZE_Z));
        Vec3 offset = {(float)x,(float)y + CraftBlock::CHUNK_SIZE_Y_OFF,(float)z};

        for (auto face: cubeFaces) {
            for (int i = 0; i < 4; i++) {
                face.points[i] += offset;
            }
            face.render = false; // Air Blocks by default
            chunkFaces.push_back(face);
        }
        p++;
    }
    needsUpdate = true;
    mesh = NewMesh(chunkFaces);
}
void ChunkObject::PutBlock(int x, int z, int y, CraftBlock::BlockType type) {
    PutBlock(x, z, y, CraftBlock::BlockByType.at(type)->ToUBlock());
}

void ChunkObject::PutBlock(int x, int z, int y, CraftBlock::UBlock block) {
    CraftBlock::Chunk::PutBlock(x,z,y,block->ToUBlock());
    needsUpdate = true;

    int blockFaceOffset = BlockFaceOffset(x,z,y); // 6 cube faces
    auto textures = block->TextureNames();
    for (int i = 0; i < 6; i++) {
        mesh->SetShowFace(blockFaceOffset+i, block->Type() != CraftBlock::Air);
        if (textures.size() > i) {
            SetTextureFromAtlas(blockFaceOffset+i, textures[i]);
        }
    }
}

void ChunkObject::Update() {
    if (needsUpdate) {
        needsUpdate = false;

        // compute faces that can be disable
        for (int x = 0; x < CraftBlock::CHUNK_SIZE_X; x++) {
            for (int z = 0; z < CraftBlock::CHUNK_SIZE_Z; z++) {
                for (int y = CraftBlock::CHUNK_SIZE_Y_OFF; y < CraftBlock::CHUNK_SIZE_Y + CraftBlock::CHUNK_SIZE_Y_OFF; y++) {
                    if (GetBlock(x,z,y)->Type() != CraftBlock::Air) {
                        int faceOffset = BlockFaceOffset(x,z,y);
                        // Lets check adjacent blocks
                        if (x > 0) {
                            mesh->SetShowFace(faceOffset + CubeFaceLeft, BlockIsTransparent(x-1, z, y));
                        }
                        if (x < CraftBlock::CHUNK_SIZE_X-1) {
                            mesh->SetShowFace(faceOffset + CubeFaceRight, BlockIsTransparent(x+1, z, y));
                        }
                        if (y > CraftBlock::CHUNK_SIZE_Y_OFF) {
                            mesh->SetShowFace(faceOffset + CubeFaceBottom, BlockIsTransparent(x, z, y-1));
                        }
                        if (y < CraftBlock::CHUNK_SIZE_Y-1) {
                            mesh->SetShowFace(faceOffset + CubeFaceTop, BlockIsTransparent(x, z, y+1));
                        }
                        if (z > 0) {
                            mesh->SetShowFace(faceOffset + CubeFaceBack, BlockIsTransparent(x, z-1, y));
                        }
                        if (z < CraftBlock::CHUNK_SIZE_Z-1) {
                            mesh->SetShowFace(faceOffset + CubeFaceFront, BlockIsTransparent(x, z+1, y));
                        }
                    }
                }
            }
        }
        mesh->invalidate();
        mesh->recompute();
        Object::Update();
    }
}