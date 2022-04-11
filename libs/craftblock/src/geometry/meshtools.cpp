#include "geometry/meshtools.h"

namespace CraftBlock {
    void defaultTexCoord(Vec2 texCoord[4]) {
        texCoord[3] = {0.f, 0.f};
        texCoord[0] = {0.f, 1.f};
        texCoord[1] = {1.f, 1.f};
        texCoord[2] = {1.f, 0.f};
    }
    void defaultColor(Vec4 colors[4]) {
        colors[0] = {1.f, 1.f, 1.f, 1.f};
        colors[1] = {1.f, 1.f, 1.f, 1.f};
        colors[2] = {1.f, 1.f, 1.f, 1.f};
        colors[3] = {1.f, 1.f, 1.f, 1.f};
    }

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
        Vec2 tx[4];
        Vec4 c[4];
        defaultTexCoord(tx);
        defaultColor(c);

        l.push_back(Face{backFace, tx, c});
        l.push_back(Face(frontFace, tx, c));
        l.push_back(Face(rightFace, tx, c));
        l.push_back(Face(leftFace, tx, c));
        l.push_back(Face(topFace, tx, c));
        l.push_back(Face(bottomFace, tx, c));

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

        Vec2 tx[4];
        Vec4 c[4];
        defaultTexCoord(tx);
        defaultColor(c);

        l.push_back(Face(backFace, tx, c));
        l.push_back(Face(frontFace, tx, c));
        l.push_back(Face(rightFace, tx, c));
        l.push_back(Face(leftFace, tx, c));
        l.push_back(Face(topFace, tx, c));
        l.push_back(Face(bottomFace, tx, c));

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

        Vec2 tx[4];
        Vec4 c[4];
        defaultTexCoord(tx);
        defaultColor(c);

        l.push_back(Face(cross0, tx, c));
        l.push_back(Face(cross1, tx, c));

        return l;
    }

    FaceList GenerateTorchFaces(float size, Vec3 center) {
        FaceList l;
        float torchSize = size * TorchThickness;
        float vsize = size * TorchHeight;

        float x0,y0,z0;
        float hsize = torchSize / 2;

        x0 = center.x - hsize;
        y0 = center.y - 0.5f;
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

        Vec2 side[4];
        Vec2 top[4];
        Vec2 bottom[4];
        Vec4 c[4];
        defaultColor(c);

        float uOffset = (1.f - TorchThickness) / 2.f;
        float vOffset = (TorchHeight);

        side[3] = {uOffset        , 1.f - TorchHeight};
        side[0] = {uOffset        , 1.f};
        side[1] = {1.f - uOffset  , 1.f};
        side[2] = {1.f - uOffset  , 1.f - TorchHeight};

        top[3] = {uOffset       , 1.f - TorchHeight};
        top[0] = {uOffset       , 1.f - TorchHeight + TorchThickness};
        top[1] = {1.f - uOffset , 1.f - TorchHeight + TorchThickness};
        top[2] = {1.f - uOffset , 1.f - TorchHeight};

        bottom[3] = {uOffset       , 1.f - TorchThickness};
        bottom[0] = {uOffset       , 1.f - TorchHeight + TorchThickness};
        bottom[1] = {1.f - uOffset , 1.f - TorchHeight + TorchThickness};
        bottom[2] = {1.f - uOffset , 1.f - TorchThickness};

        l.push_back(Face(backFace, side, c));
        l.push_back(Face(frontFace, side, c));
        l.push_back(Face(rightFace, side, c));
        l.push_back(Face(leftFace, side, c));
        l.push_back(Face(topFace, top, c));
        l.push_back(Face(bottomFace, bottom, c));

        return l;
    }
}