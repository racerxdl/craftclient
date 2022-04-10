#pragma once

#include <vector>
#include <cassert>

#include "vectors.h"

const Vec4 emptyColors[4];
const Vec2 emptyTexCoord[4];
const Vec3 emptyPoints[4];

class Face {
public:
    Vec3 points[4];
    Vec2 texCoord[4];
    Vec4 color[4];
    bool render = true;

    Face() {}
    Face(const Vec3 points[4], const Vec2 texCoord[4], const Vec4 color[4]) {
        for (int i = 0; i < 4; i++) {
            this->points[i] = points[i];
            this->texCoord[i] = texCoord[i];
            this->color[i] = color[i];
        }
    }
    Face(const Vec3 points[4], const Vec2 texCoord[4]) : Face(points, texCoord, emptyColors) {}
    Face(const Vec3 points[4], const Vec4 color[4]) : Face(points, emptyTexCoord, color) {}
    Face(const Vec3 points[4]) : Face(points, emptyTexCoord, emptyColors) {}
};

typedef std::vector<Face> FaceList;