#pragma once

#include <geometry/face.h>

namespace CraftBlock {
    constexpr int CubeFaceBack = 0;
    constexpr int CubeFaceFront = 1;
    constexpr int CubeFaceRight = 2;
    constexpr int CubeFaceLeft = 3;
    constexpr int CubeFaceTop = 4;
    constexpr int CubeFaceBottom = 5;
    constexpr float TorchThickness = 0.125f;
    constexpr float TorchHeight = 0.625f;

    FaceList GenerateCubeFaces(float size, Vec3 center={0,0,0});
    // Same as GenerateCubeFaces but for liquids
    // Basically 80% of size for y
    FaceList GenerateLiquidCubeFaces(float size, Vec3 center={0,0,0});
    FaceList GenerateXSprite(float size, Vec3 center={0,0,0});
    FaceList GenerateTorchFaces(float size, Vec3 center={0,0,0});
}