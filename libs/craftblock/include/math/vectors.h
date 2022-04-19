#pragma once

#include <cstddef>
#include <ostream>

#include "cond.h"

namespace CraftBlock {
template <size_t N>
class BaseMat;

template <size_t N, typename vec_t = float>
class alignas(vec_t) TypedBaseVec {
   private:
    template <signed int P>
    struct aux {
        operator vec_t() const {
            return ((vec_t*)this)[P];
        }
        operator vec_t&() {
            return ((vec_t*)this)[P];
        }
        vec_t operator=(vec_t other) {
            return (((vec_t*)this)[P] = other);
        }
        vec_t operator+=(vec_t other) {
            vec_t v = ((vec_t*)this)[P] + other;
            return (((vec_t*)this)[P] = v);
        }
        vec_t operator-=(vec_t other) {
            vec_t v = ((vec_t*)this)[P] - other;
            return (((vec_t*)this)[P] = v);
        }
        vec_t operator*=(vec_t other) {
            vec_t v = ((vec_t*)this)[P] * other;
            return (((vec_t*)this)[P] = v);
        }
        vec_t operator/=(vec_t other) {
            vec_t v = ((vec_t*)this)[P] / other;
            return (((vec_t*)this)[P] = v);
        }
        vec_t operator|=(vec_t other) {
            vec_t v = ((vec_t*)this)[P] | other;
            return (((vec_t*)this)[P] = v);
        }
        vec_t operator&=(vec_t other) {
            vec_t v = ((vec_t*)this)[P] | other;
            return (((vec_t*)this)[P] = v);
        }
    };

    vec_t sortingModulus() const {  // Modulus used for sorting vectors in maps
        vec_t r;
        for (int i = 0; i < N; i++) {
            r += memory[i];
        }
        return r;
    }

   public:
    // Optional fields!
    // For some unknown reason, this should be BEFORE memory declartion
    [[no_unique_address]] cond_t<(N > 0), aux<0>> x;
    [[no_unique_address]] cond_t<(N > 1), aux<1>> y;
    [[no_unique_address]] cond_t<(N > 2), aux<2>> z;
    [[no_unique_address]] cond_t<(N > 3), aux<3>> w;

    [[no_unique_address]] cond_t<(N > 0), aux<0>> u;
    [[no_unique_address]] cond_t<(N > 1), aux<1>> v;

    [[no_unique_address]] cond_t<(N > 0), aux<0>> r;
    [[no_unique_address]] cond_t<(N > 1), aux<1>> g;
    [[no_unique_address]] cond_t<(N > 2), aux<2>> b;
    [[no_unique_address]] cond_t<(N > 3), aux<3>> a;

    vec_t memory[N];

    constexpr size_t size() { return N; }

    TypedBaseVec<N, vec_t>(std::initializer_list<vec_t> vals) {
        for (int i = 0; i < N; i++) {
            memory[i] = vals.begin()[i];
        }
    }
    TypedBaseVec<N, vec_t>(const TypedBaseVec<N, vec_t>& o) {
        for (int i = 0; i < N; i++) {
            memory[i] = o[i];
        }
    }
    TypedBaseVec<N, vec_t>() {
        for (int i = 0; i < N; i++) {
            memory[i] = 0.f;
        }
    }

    vec_t operator[](int idx) const { return memory[idx]; }

    vec_t& operator[](int idx) { return memory[idx]; }

    vec_t Length() const {
        return sqrt(InnerProduct());
    }

    vec_t InnerProduct() const {
        vec_t r;
        for (int i = 0; i < N; i++) {
            r += memory[i] * memory[i];
        }
        return r;
    }
    TypedBaseVec<N, vec_t> CrossProduct(const TypedBaseVec<N, vec_t>&) const;

    TypedBaseVec<N, vec_t> Normalize() const {
        vec_t s = 1.f / Length();
        return (*this) * s;
    }

    void Scale(TypedBaseVec<N, vec_t> s) {
        for (int i = 0; i < N; i++) {
            this->memory[i] *= s[i];
        }
    }

    void operator=(const TypedBaseVec<N, vec_t>& v1) {
        for (int i = 0; i < N; i++) {
            this->memory[i] = v1[i];
        }
    }

    void operator+=(const TypedBaseVec<N, vec_t>& v1) {
        for (int i = 0; i < N; i++) {
            this->memory[i] += v1[i];
        }
    }

    void operator-=(const TypedBaseVec<N, vec_t>& v1) {
        for (int i = 0; i < N; i++) {
            this->memory[i] -= v1[i];
        }
    }

    TypedBaseVec<N, vec_t> operator*(const BaseMat<N> m) const;

    // Friend Operators
    friend TypedBaseVec<N, vec_t> operator+(const TypedBaseVec<N, vec_t>& v0, const TypedBaseVec<N, vec_t>& v1) {
        TypedBaseVec<N, vec_t> vo;
        for (int i = 0; i < N; i++) {
            vo[i] = v0[i] + v1[i];
        }
        return vo;
    }

    friend TypedBaseVec<N, vec_t> operator-(const TypedBaseVec<N, vec_t>& v0, const TypedBaseVec<N, vec_t>& v1) {
        TypedBaseVec<N, vec_t> vo;
        for (int i = 0; i < N; i++) {
            vo[i] = v0[i] - v1[i];
        }
        return vo;
    }

    friend TypedBaseVec<N, vec_t> operator*(const TypedBaseVec<N, vec_t>& v0, const vec_t s) {
        TypedBaseVec<N, vec_t> vo;
        for (int i = 0; i < N; i++) {
            vo[i] = v0[i] * s;
        }
        return vo;
    }

    friend TypedBaseVec<N, vec_t> operator/(const TypedBaseVec<N, vec_t>& v0, const vec_t s) {
        TypedBaseVec<N, vec_t> vo;
        for (int i = 0; i < N; i++) {
            vo[i] = v0[i] / s;
        }
        return vo;
    }

    friend TypedBaseVec<N, vec_t> operator/(const vec_t s, const TypedBaseVec<N, vec_t>& v0) {
        TypedBaseVec<N, vec_t> vo;
        for (int i = 0; i < N; i++) {
            vo[i] = s / v0[i];
        }
        return vo;
    }

    friend bool operator==(const TypedBaseVec<N, vec_t>& v0, const TypedBaseVec<N, vec_t>& v1) {
        for (int i = 0; i < N; i++) {
            if (v0[i] != v1[i]) {
                return false;
            }
        }
        return true;
    }

    friend bool operator<(const TypedBaseVec<N, vec_t>& v0, const TypedBaseVec<N, vec_t>& v1) {
        return v0.sortingModulus() < v1.sortingModulus();
    }

    friend bool operator!=(const TypedBaseVec<N, vec_t>& v0, const TypedBaseVec<N, vec_t>& v1) {
        return !(v0 == v1);
    }
};

template <size_t N>
class alignas(float) BaseVec : public TypedBaseVec<N, float> {
   public:
    using TypedBaseVec<N, float>::TypedBaseVec;
    using TypedBaseVec<N, float>::CrossProduct;
    BaseVec<N>(const TypedBaseVec<N, float> &v) : TypedBaseVec<N, float>(v) {}
    const float* AsFloatArray() const { return &this->memory[0]; }
    float* AsFloatArray() { return &this->memory[0]; }
};


template <size_t N>
class alignas(float) BaseIVec : public TypedBaseVec<N, int32_t> {
   public:
    using TypedBaseVec<N, int32_t>::TypedBaseVec;
    using TypedBaseVec<N, int32_t>::CrossProduct;
    BaseIVec<N>(const TypedBaseVec<N, int32_t> &v) : TypedBaseVec<N, int32_t>(v) {}
    const int32_t* AsInt32Array() const { return &this->memory[0]; }
    int32_t* AsInt32Array() { return &this->memory[0]; }
};


template <size_t N, typename vec_t = float>
std::ostream& operator<<(std::ostream& stream, const TypedBaseVec<N, vec_t>& v);

typedef BaseVec<2> Vec2;
typedef BaseVec<3> Vec3;
typedef BaseVec<4> Vec4;

typedef BaseIVec<2> VecI2;
typedef BaseIVec<3> VecI3;
typedef BaseIVec<4> VecI4;

static_assert(sizeof(BaseVec<2>) == sizeof(float) * 2);
static_assert(sizeof(BaseVec<3>) == sizeof(float) * 3);
static_assert(sizeof(BaseVec<4>) == sizeof(float) * 4);

static_assert(sizeof(Vec2) == sizeof(float) * 2);
static_assert(sizeof(Vec3) == sizeof(float) * 3);
static_assert(sizeof(Vec4) == sizeof(float) * 4);
}  // namespace CraftBlock
