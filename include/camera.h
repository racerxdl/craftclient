#pragma once

#include <math/matrix.h>

class Camera {
    CraftBlock::Vec3 pos;
    CraftBlock::Vec3 direction;
    CraftBlock::Vec3 right;
    CraftBlock::Vec3 camFront;
    CraftBlock::Mat4 view;
    bool needsMatrixUpdate = true;
    CraftBlock::Vec2 pitchYaw;

    template<size_t N>
    struct posgetset {
        operator float() const {
            return ((const Camera *)this)->pos[N];
        }
        float operator = (float other) {
            if (((Camera *)this)->pos[N] != other) {
                ((Camera *)this)->needsMatrixUpdate = true;
            }
            return ((Camera *)this)->pos[N] = other;
        }
    };

    template<size_t N>
    struct pitchyawgetset {
        operator float() const {
            return ((const Camera *)this)->pitchYaw[N];
        }
        float operator = (float other) {
            if (((Camera *)this)->pitchYaw[N] != other) {
                ((Camera *)this)->needsMatrixUpdate = true;
            }
            return ((Camera *)this)->pitchYaw[N] = other;
        }
        float operator += (float other) {
            ((Camera *)this)->needsMatrixUpdate = true;
            return ((Camera *)this)->pitchYaw[N] = ((Camera *)this)->pitchYaw[N] + other;
        }
    };
public:
    bool update();
    CraftBlock::Mat4 getMatrix() { return view; }
    void SetPos(const CraftBlock::Vec3 &p) { pos = p; needsMatrixUpdate = true; }
    CraftBlock::Vec3 GetPos() const { return pos; }
    CraftBlock::Vec3 GetCamFront() const { return camFront; }

    // Magic Fields
    [[no_unique_address]] posgetset<0> x;
    [[no_unique_address]] posgetset<1> y;
    [[no_unique_address]] posgetset<2> z;

    [[no_unique_address]] pitchyawgetset<0> pitch;
    [[no_unique_address]] pitchyawgetset<1> yaw;

};