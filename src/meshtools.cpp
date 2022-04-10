#include "meshtools.h"

Vec4 whiteFace[4] = {
    {1.f, 1.f, 1.f, 1.f},
    {1.f, 1.f, 1.f, 1.f},
    {1.f, 1.f, 1.f, 1.f},
    {1.f, 1.f, 1.f, 1.f},
};
Vec4 redFace[4] = {
    {1.f, 0.f, 0.f, 1.f},
    {1.f, 0.f, 0.f, 1.f},
    {1.f, 0.f, 0.f, 1.f},
    {1.f, 0.f, 0.f, 1.f},
};
Vec2 texCoord[4] = {
    {0.f, 0.f},
    {0.f, 1.f},
    {1.f, 1.f},
    {1.f, 0.f},
};

FaceList GenerateCubeFaces(float size, Vec3 center) {
    FaceList l;

    float x0,y0,z0;
    float hsize = size / 2;

    x0 = center.x - hsize;
    y0 = center.y - hsize;
    z0 = center.z - hsize;

    Vec3 backFace[4] = {
        {x0+size,   y0,     z0},
        {x0,        y0,     z0},
        {x0,        y0+size,z0},
        {x0+size,   y0+size,z0},
    };
    Vec3 frontFace[4] = {
        {x0     ,   y0      ,z0+size},
        {x0+size,   y0      ,z0+size},
        {x0+size,   y0+size ,z0+size},
        {x0,        y0+size ,z0+size},
    };
    Vec3 bottomFace[4] = {
        {x0+size,   y0      ,z0},
        {x0+size,   y0      ,z0+size},
        {x0     ,   y0      ,z0+size},
        {x0     ,   y0      ,z0},
    };
    Vec3 topFace[4] = {
        {x0     ,   y0+size,z0},
        {x0     ,   y0+size,z0+size},
        {x0+size,   y0+size,z0+size},
        {x0+size,   y0+size,z0},
    };
    Vec3 leftFace[4] = {
        {x0     ,   y0      ,z0},
        {x0     ,   y0      ,z0+size},
        {x0     ,   y0+size ,z0+size},
        {x0     ,   y0+size ,z0},
    };
    Vec3 rightFace[4] = {
        {x0+size,   y0      ,z0+size},
        {x0+size,   y0      ,z0},
        {x0+size,   y0+size ,z0},
        {x0+size,   y0+size ,z0+size},
    };

    l.push_back(Face(backFace, texCoord, whiteFace));
    l.push_back(Face(frontFace, texCoord, whiteFace));
    l.push_back(Face(rightFace, texCoord, whiteFace));
    l.push_back(Face(leftFace, texCoord, whiteFace));
    l.push_back(Face(topFace, texCoord, whiteFace));
    l.push_back(Face(bottomFace, texCoord, whiteFace));

    return l;
}