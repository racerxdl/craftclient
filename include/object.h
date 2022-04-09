#pragma once

#include <memory>

#include "mesh.h"
#include "renderstate.h"
#include "texture.h"

class Object: public Renderable {
private:
    Mesh mesh;
    Vec3 pos;
    Vec3 rot;
    Mat4 m;
    Texture tex;
    bool needsMatrixUpdate = true;

    template<size_t N>
    struct posgetset {
        operator float() {
            return ((Object *)this)->pos[N];
        }
        float operator = (float other) {
            if (((Object *)this)->pos[N] != other) {
                ((Object *)this)->needsMatrixUpdate = true;
            }
            return ((Object *)this)->pos[N] = other;
        }
    };
    template<size_t N>
    struct rotgetset {
        operator float() {
            return ((Object *)this)->rot[N];
        }
        float operator = (float other) {
            if (((Object *)this)->rot[N] != other) {
                ((Object *)this)->needsMatrixUpdate = true;
            }
            return ((Object *)this)->rot[N] = other;
        }
    };

public:
    Object(Mesh mesh) : mesh(mesh), needsMatrixUpdate(true) {}
    Object(Mesh mesh, Texture tex) : mesh(mesh), tex(tex), needsMatrixUpdate(true) {}
    Object(const Object &b) { *this = b; }

    void Update();
    void SetTexture(Texture tex) { this->tex = tex; }
    void Render(const RenderState &);
    void SetPos(const Vec3 &pos) { needsMatrixUpdate = true; this->pos = pos; }
    void SetRotation(const Vec3 &rot) { needsMatrixUpdate = true; this->rot = rot; }

    Vec3 GetPos() const { return pos; }
    Vec3 GetRotation() const { return rot; }

    // Magic fields
    [[no_unique_address]] posgetset<0> x;
    [[no_unique_address]] posgetset<1> y;
    [[no_unique_address]] posgetset<2> z;

    [[no_unique_address]] rotgetset<0> roll;
    [[no_unique_address]] rotgetset<1> pitch;
    [[no_unique_address]] rotgetset<2> yaw;
};