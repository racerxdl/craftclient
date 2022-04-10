#include "geometry/meshtools.h"

namespace CraftBlock {
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

    FaceList GenerateLiquidCubeFaces(float size, Vec3 center) {
        FaceList l;

        float x0,y0,z0;
        float hsize = size / 2;
        float vsize = size * 0.9f; // 80% size

        x0 = center.x - hsize;
        y0 = center.y - hsize;
        z0 = center.z - hsize;

        Vec3 backFace[4] = {
            {x0+size,   y0,     z0},
            {x0,        y0,     z0},
            {x0,        y0+vsize,z0},
            {x0+size,   y0+vsize,z0},
        };
        Vec3 frontFace[4] = {
            {x0     ,   y0      ,z0+size},
            {x0+size,   y0      ,z0+size},
            {x0+size,   y0+vsize ,z0+size},
            {x0,        y0+vsize ,z0+size},
        };
        Vec3 bottomFace[4] = {
            {x0+size,   y0      ,z0},
            {x0+size,   y0      ,z0+size},
            {x0     ,   y0      ,z0+size},
            {x0     ,   y0      ,z0},
        };
        Vec3 topFace[4] = {
            {x0     ,   y0+vsize,z0},
            {x0     ,   y0+vsize,z0+size},
            {x0+size,   y0+vsize,z0+size},
            {x0+size,   y0+vsize,z0},
        };
        Vec3 leftFace[4] = {
            {x0     ,   y0      ,z0},
            {x0     ,   y0      ,z0+size},
            {x0     ,   y0+vsize ,z0+size},
            {x0     ,   y0+vsize ,z0},
        };
        Vec3 rightFace[4] = {
            {x0+size,   y0      ,z0+size},
            {x0+size,   y0      ,z0},
            {x0+size,   y0+vsize ,z0},
            {x0+size,   y0+vsize ,z0+size},
        };

        l.push_back(Face(backFace, texCoord, whiteFace));
        l.push_back(Face(frontFace, texCoord, whiteFace));
        l.push_back(Face(rightFace, texCoord, whiteFace));
        l.push_back(Face(leftFace, texCoord, whiteFace));
        l.push_back(Face(topFace, texCoord, whiteFace));
        l.push_back(Face(bottomFace, texCoord, whiteFace));

        return l;
    }

    FaceList GenerateXSprite(float size, Vec3 center) {
        FaceList l;

        float x0,y0,z0;
        float hsize = size / 2;

        x0 = center.x - hsize;
        y0 = center.y - hsize;
        z0 = center.z - hsize;

        Vec3 cross0[4] = {
            {x0,        y0,     z0     },
            {x0+size,   y0,     z0+size},
            {x0+size,   y0+size,z0+size},
            {x0     ,   y0+size,z0     },
        };

        Vec3 cross1[4] = {
            {x0+size,   y0,     z0     },
            {x0,        y0,     z0+size},
            {x0     ,   y0+size,z0+size},
            {x0+size,   y0+size,z0     },
        };

        l.push_back(cross0);
        l.push_back(cross1);

        return l;
    }

    FaceList GenerateTorchFaces(float size, Vec3 center) {
        FaceList l;
        float torchSize = size * TorchThickness;
        float vsize = size * TorchHeight;

        float x0,y0,z0;
        float hsize = torchSize / 2;

        x0 = center.x - hsize;
        y0 = center.y - vsize;
        z0 = center.z - hsize;

        Vec3 backFace[4] = {
            {x0+torchSize,  y0          ,z0},
            {x0,            y0          ,z0},
            {x0,            y0+vsize    ,z0},
            {x0+torchSize,  y0+vsize    ,z0},
        };
        Vec3 frontFace[4] = {
            {x0             ,y0         ,z0+torchSize},
            {x0+torchSize   ,y0         ,z0+torchSize},
            {x0+torchSize   ,y0+vsize   ,z0+torchSize},
            {x0             ,y0+vsize   ,z0+torchSize},
        };
        Vec3 bottomFace[4] = {
            {x0+torchSize   ,y0         ,z0},
            {x0+torchSize   ,y0         ,z0+torchSize},
            {x0             ,y0         ,z0+torchSize},
            {x0             ,y0         ,z0},
        };
        Vec3 topFace[4] = {
            {x0             ,y0+vsize   ,z0},
            {x0             ,y0+vsize   ,z0+torchSize},
            {x0+torchSize   ,y0+vsize   ,z0+torchSize},
            {x0+torchSize   ,y0+vsize   ,z0},
        };
        Vec3 leftFace[4] = {
            {x0             ,y0         ,z0},
            {x0             ,y0         ,z0+torchSize},
            {x0             ,y0+vsize   ,z0+torchSize},
            {x0             ,y0+vsize   ,z0},
        };
        Vec3 rightFace[4] = {
            {x0+torchSize,   y0         ,z0+torchSize},
            {x0+torchSize,   y0         ,z0},
            {x0+torchSize,   y0+vsize   ,z0},
            {x0+torchSize,   y0+vsize   ,z0+torchSize},
        };

        l.push_back(Face(backFace, texCoord, whiteFace));
        l.push_back(Face(frontFace, texCoord, whiteFace));
        l.push_back(Face(rightFace, texCoord, whiteFace));
        l.push_back(Face(leftFace, texCoord, whiteFace));
        l.push_back(Face(topFace, texCoord, whiteFace));
        l.push_back(Face(bottomFace, texCoord, whiteFace));

        return l;
    }
}