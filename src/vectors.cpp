#include "vectors.h"
#include "matrix.h"

#include <iostream>
#include <cassert>

template<size_t N> BaseVec<N> BaseVec<N>::CrossProduct(const BaseVec<N> &b) const {
    auto a = memory;
    BaseVec<N> r;

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

template<size_t N> BaseVec<N> BaseVec<N>::operator*(const BaseMat<N> M) const {
    BaseVec<N> r;

    for (int j=0; j<N; j++) {
        r[j] = 0.f;
        for (int i=0; i<N; i++) {
            r[j] += M[i][j] * memory[i];
        }
    }

    return r;
}

template<size_t N> std::ostream& operator<< (std::ostream& stream, const BaseVec<N>& v) {
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

template std::ostream& operator<< (std::ostream& stream, const BaseVec<2>& v);
template std::ostream& operator<< (std::ostream& stream, const BaseVec<3>& v);
template std::ostream& operator<< (std::ostream& stream, const BaseVec<4>& v);

template BaseVec<2> BaseVec<2>::operator*(BaseMat<2> M) const;
template BaseVec<3> BaseVec<3>::operator*(BaseMat<3> M) const;
template BaseVec<4> BaseVec<4>::operator*(BaseMat<4> M) const;

template BaseVec<2> BaseVec<2>::CrossProduct(const BaseVec<2> &b) const;
template BaseVec<3> BaseVec<3>::CrossProduct(const BaseVec<3> &b) const;