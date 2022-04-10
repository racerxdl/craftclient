#pragma once

#include <cstddef>
#include <ostream>

#include "cond.h"

namespace CraftBlock {
    template <size_t N> class BaseMat;

    template <size_t N>
    class alignas(float) BaseVec {
    private:
        template<signed int P>
        struct aux {
            operator float() {
                return ((float *)this)[P];
            }
            float operator =(float other) {
                return (((float *)this)[P] = other);
            }
            float operator +=(float other) {
                float v = ((float *)this)[P] + other;
                return (((float *)this)[P] = v);
            }
            float operator -=(float other) {
                float v = ((float *)this)[P] - other;
                return (((float *)this)[P] = v);
            }
            float operator *=(float other) {
                float v = ((float *)this)[P] * other;
                return (((float *)this)[P] = v);
            }
            float operator /=(float other) {
                float v = ((float *)this)[P] / other;
                return (((float *)this)[P] = v);
            }
        };

        float sortingModulus() const { // Modulus used for sorting vectors in maps
            float r;
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

        float memory[N];

        constexpr size_t size() { return N; }

        BaseVec<N>() { for (int i = 0; i < N; i++) { memory[i] = 0.f; }}
        BaseVec<N>(std::initializer_list<float> vals) { for (int i = 0; i < N; i++) { memory[i] = vals.begin()[i]; }}

        const float *AsFloatArray() const { return &memory[0]; }
        float *AsFloatArray() { return &memory[0]; }

        float operator[](int idx) const { return memory[idx]; }

        float &operator[](int idx) { return memory[idx]; }

        float Length() const {
            return sqrt(InnerProduct());
        }

        float InnerProduct() const {
            float r;
            for (int i = 0; i < N; i++) {
                r += memory[i] * memory[i];
            }
            return r;
        }
        BaseVec<N> CrossProduct(const BaseVec<N> &) const;

        BaseVec<N> Normalize() const {
            float s = 1.f / Length();
            return (*this) * s;
        }

        void operator=(const BaseVec<N>& v1) {
            for(int i = 0; i < N; i++) {
                this->memory[i] = v1[i];
            }
        }

        void operator+=(const BaseVec<N> &v1) {
            for(int i = 0; i < N; i++) {
                this->memory[i] += v1[i];
            }
        }

        void operator-=(const BaseVec<N> &v1) {
            for(int i = 0; i < N; i++) {
                this->memory[i] -= v1[i];
            }
        }

        BaseVec<N> operator*(const BaseMat<N> m) const;

        // Friend Operators
        friend BaseVec<N> operator+(const BaseVec<N>& v0, const BaseVec<N>& v1) {
            BaseVec<N> vo;
            for (int i = 0; i < N; i++) {
                vo[i] = v0[i] + v1[i];
            }
            return vo;
        }

        friend BaseVec<N> operator-(const BaseVec<N>& v0, const BaseVec<N>& v1) {
            BaseVec<N> vo;
            for (int i = 0; i < N; i++) {
                vo[i] = v0[i] - v1[i];
            }
            return vo;
        }

        friend BaseVec<N> operator*(const BaseVec<N>& v0, const float s) {
            BaseVec<N> vo;
            for (int i = 0; i < N; i++) {
                vo[i] = v0[i] * s;
            }
            return vo;
        }

        friend BaseVec<N> operator/(const BaseVec<N>& v0, const float s) {
            BaseVec<N> vo;
            for (int i = 0; i < N; i++) {
                vo[i] = v0[i] / s;
            }
            return vo;
        }

        friend bool operator==(const BaseVec<N>& v0, const BaseVec<N>& v1) {
            for (int i = 0; i < N; i++) {
                if (v0[i] != v1[i]) {
                    return false;
                }
            }
            return true;
        }

        friend bool operator<(const BaseVec<N>& v0, const BaseVec<N>& v1) {
            return v0.sortingModulus() < v1.sortingModulus();
        }

        friend bool operator!= (const BaseVec<N>& v0, const BaseVec<N>& v1) {
            return !(v0 == v1);
        }
    };

    template<size_t N> std::ostream& operator<< (std::ostream& stream, const BaseVec<N>& v);

    typedef BaseVec<2> Vec2;
    typedef BaseVec<3> Vec3;
    typedef BaseVec<4> Vec4;

    static_assert(sizeof(BaseVec<2>) == sizeof(float) * 2);
    static_assert(sizeof(BaseVec<3>) == sizeof(float) * 3);
    static_assert(sizeof(BaseVec<4>) == sizeof(float) * 4);

    static_assert(sizeof(Vec2) == sizeof(float) * 2);
    static_assert(sizeof(Vec3) == sizeof(float) * 3);
    static_assert(sizeof(Vec4) == sizeof(float) * 4);
}
