#include "mineblock.h"
#include <craftblock.h>

using namespace CraftBlock;

Vec3 blockOffset(int x, int z, int y) {
    return Vec3{(float)x,(float)y,(float)z};
}

ChunkObject::ChunkObject() : Object(), Chunk() {
    auto chunkFaces = FaceList();
    chunkFaces.reserve(6 * blocks.size()); // Pre-reserve for maximum geometry
    faceCulling = false;
    needsUpdate = true;
    mesh = NewMesh(chunkFaces);
}
void ChunkObject::PutBlock(int x, int z, int y, CraftBlock::BlockType type) {
    PutBlock(x, z, y, CraftBlock::BlockByType.at(type)->ToUBlock());
}

void ChunkObject::PutBlock(int x, int z, int y, CraftBlock::UBlock block) {
    CraftBlock::Chunk::PutBlock(x,z,y,block->ToUBlock());
    needsUpdate = true;
}

void ChunkObject::Update() {
    if (needsUpdate) {
        needsUpdate = false;
        mesh->faces.resize(0); // Clear without changing pre-allocated capacity

        // compute faces
        for (int x = 0; x < CraftBlock::CHUNK_SIZE_X; x++) {
            for (int z = 0; z < CraftBlock::CHUNK_SIZE_Z; z++) {
                for (int y = CraftBlock::CHUNK_SIZE_Y_OFF; y < CraftBlock::CHUNK_SIZE_Y + CraftBlock::CHUNK_SIZE_Y_OFF; y++) {
                    auto block = GetBlock(x,z,y);
                    if (block->Type() != CraftBlock::Air) { // Air doesnt need to have faces.
                        // Get Geometry Faces
                        auto textures = block->TextureNames();
                        auto faces = CraftBlock::BlockGeometries[block->Type()].faces;
                        auto offset = blockOffset(x,z,y);

                        assert(textures.size() == 1 || textures.size() == faces.size());
                        // Move block to the right position and pre-fill texture coordinates
                        for (int i = 0; i < faces.size(); i++) {
                            auto texName = (textures.size() == 1) ? textures[0] : textures[i];
                            auto bounds = atlas->GetBounds(texName);

                            // Texture
                            faces[i].texCoord[3] = Vec2{bounds.x, bounds.y};
                            faces[i].texCoord[0] = Vec2{bounds.x, bounds.w};
                            faces[i].texCoord[1] = Vec2{bounds.z, bounds.w};
                            faces[i].texCoord[2] = Vec2{bounds.z, bounds.y};

                            // Coordinate
                            for (int t = 0; t < 4; t++) {
                                faces[i].points[t] += offset;
                            }
                        }

                        if (block->IsTransparent()) { // Add all faces
                            mesh->faces.insert(mesh->faces.begin(), faces.begin(), faces.end());
                        } else {
                            // Lets check adjacent blocks
                            if (x == 0 || (x > 0 && BlockIsTransparent(x-1, z, y))) {
                                mesh->faces.push_back(faces[CubeFaceLeft]);
                            }
                            if (x == CraftBlock::CHUNK_SIZE_X-1 || (x < CraftBlock::CHUNK_SIZE_X-1 && BlockIsTransparent(x+1, z, y))) {
                                mesh->faces.push_back(faces[CubeFaceRight]);
                            }
                            if (y == CraftBlock::CHUNK_SIZE_Y_OFF || (y > CraftBlock::CHUNK_SIZE_Y_OFF && BlockIsTransparent(x, z, y-1))) {
                                mesh->faces.push_back(faces[CubeFaceBottom]);
                            }
                            if (y == CraftBlock::CHUNK_SIZE_Y-1 || (y < CraftBlock::CHUNK_SIZE_Y-1 && BlockIsTransparent(x, z, y+1))) {
                                mesh->faces.push_back(faces[CubeFaceTop]);
                            }
                            if (z == 0 || (z > 0 && BlockIsTransparent(x, z-1, y))) {
                                mesh->faces.push_back(faces[CubeFaceBack]);
                            }
                            if (z == CraftBlock::CHUNK_SIZE_Z-1 || (z < CraftBlock::CHUNK_SIZE_Z-1 && BlockIsTransparent(x, z+1, y))) {
                                mesh->faces.push_back(faces[CubeFaceFront]);
                            }
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