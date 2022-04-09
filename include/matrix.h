#pragma once

#include <cmath>
#include <cstddef>
#include "vectors.h"

template <size_t N>
class BaseMat {
private:
public:
    BaseVec<N> memory[N];

    BaseVec<N> &operator[](int idx) { return memory[idx]; }
    const BaseVec<N> &operator[](int idx) const { return memory[idx]; }

    constexpr size_t size() { return N; }

    void Identity() {
        for (int i=0; i<N; i++) {
            for (int j=0; j<N; j++) {
                memory[i][j] = i==j ? 1.f : 0.f;
            }
        }
    }

    void RotateX(float angle);
    void RotateY(float angle);
    void RotateZ(float angle);
    void Translate(float x, float y, float z);

    void operator=(const BaseMat<N> &b) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                this->memory[i][j] = b[i][j];
            }
        }
    }

    BaseMat<N> operator *(const BaseMat<N> &) const;
    BaseMat<N> operator *=(const BaseMat<N> &m) { *this = *this * m; return *this; }

    const float *AsFloatArray() const { // This should work. Change if it doesnt
        return memory[0].AsFloatArray();
    }
};

template<size_t N> std::ostream& operator<< (std::ostream& stream, const BaseMat<N>& v);

class Mat3: public BaseMat<3> {
public:
    void operator=(const BaseMat<3>& b) { *((BaseMat<3>*)this) = b; }
};

class Mat4: public BaseMat<4> {
public:
    Mat4() {}
    Mat4(const BaseMat<4> &m) { for (int i = 0; i < 4; i++) { memory[i] = m[i]; }}
    void Ortho(float l, float r, float b, float t, float n, float f);
    void Perspective(float y_fov, float aspect, float n, float f);

    void operator=(const BaseMat<4>& b) { *((BaseMat<4>*)this) = b; }
};

Mat4 OrthoProjection(float l, float r, float b, float t, float n, float f);
Mat4 PerspectiveProjection(float y_fov, float aspect, float n, float f);
Mat4 LookAt(Vec3 const eye, Vec3 const center, Vec3 const up);

static_assert(sizeof(BaseMat<2>) == sizeof(float) * 4);
static_assert(sizeof(BaseMat<3>) == sizeof(float) * 9);
static_assert(sizeof(BaseMat<4>) == sizeof(float) * 16);

static_assert(sizeof(Mat3) == sizeof(float) * 9);
static_assert(sizeof(Mat4) == sizeof(float) * 16);