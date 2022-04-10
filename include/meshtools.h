#pragma once

#include "face.h"

constexpr int CubeFaceBack = 0;
constexpr int CubeFaceFront = 1;
constexpr int CubeFaceRight = 2;
constexpr int CubeFaceLeft = 3;
constexpr int CubeFaceTop = 4;
constexpr int CubeFaceBottom = 5;

FaceList GenerateCubeFaces(float size, Vec3 center={0,0,0});

