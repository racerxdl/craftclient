#include "vectors.h"
#include "matrix.h"

#include <iostream>
#include <cassert>

namespace CraftBlock {
    template<size_t N, typename vec_t> TypedBaseVec<N, vec_t> TypedBaseVec<N, vec_t>::CrossProduct(const TypedBaseVec<N, vec_t> &b) const {
        auto a = memory;
        TypedBaseVec<N, vec_t> r;

        assert(N == 2 || N == 3); // Only for Vec2/Vec3

        if (N == 2) {
            r[0] = a[0]*b[1];
            r[1] = a[1]*b[0];
        }

        if (N == 3) {
            r[0] = a[1]*b[2] - a[2]*b[1];
            r[1] = a[2]*b[0] - a[0]*b[2];
            r[2] = a[0]*b[1] - a[1]*b[0];
        }

        return r;
    }

    template<size_t N, typename vec_t> TypedBaseVec<N, vec_t> TypedBaseVec<N, vec_t>::operator*(const BaseMat<N> M) const {
        TypedBaseVec<N, vec_t> r;

        for (int j=0; j<N; j++) {
            r[j] = 0.f;
            for (int i=0; i<N; i++) {
                r[j] += M[i][j] * memory[i];
            }
        }

        return r;
    }

    template<size_t N, typename vec_t> std::ostream& operator<< (std::ostream& stream, const TypedBaseVec<N, vec_t>& v) {
        stream << "Vec" << N << "(";
        for (int i = 0; i < N; i++) {
            stream << v[i];
            if (i != N-1) {
                stream << ",";
            }
        }
        stream << ")";
        return stream;
    }

    template std::ostream& operator<< (std::ostream& stream, const TypedBaseVec<2, float>& v);
    template std::ostream& operator<< (std::ostream& stream, const TypedBaseVec<3, float>& v);
    template std::ostream& operator<< (std::ostream& stream, const TypedBaseVec<4, float>& v);

    template std::ostream& operator<< (std::ostream& stream, const TypedBaseVec<2, int32_t>& v);
    template std::ostream& operator<< (std::ostream& stream, const TypedBaseVec<3, int32_t>& v);
    template std::ostream& operator<< (std::ostream& stream, const TypedBaseVec<4, int32_t>& v);

    template TypedBaseVec<2, float> TypedBaseVec<2, float>::operator*(BaseMat<2> M) const;
    template TypedBaseVec<3, float> TypedBaseVec<3, float>::operator*(BaseMat<3> M) const;
    template TypedBaseVec<4, float> TypedBaseVec<4, float>::operator*(BaseMat<4> M) const;

    template TypedBaseVec<2, float> TypedBaseVec<2, float>::CrossProduct(const TypedBaseVec<2, float> &b) const;
    template TypedBaseVec<3, float> TypedBaseVec<3, float>::CrossProduct(const TypedBaseVec<3, float> &b) const;

    template TypedBaseVec<2, int32_t> TypedBaseVec<2, int32_t>::CrossProduct(const TypedBaseVec<2, int32_t> &b) const;
    template TypedBaseVec<3, int32_t> TypedBaseVec<3, int32_t>::CrossProduct(const TypedBaseVec<3, int32_t> &b) const;
}