#include "matrix.h"



template<size_t N> void BaseMat<N>::RotateX(float angle) {
    if (N < 3) {
        return;
    }

    float s = sinf(angle);
    float c = cosf(angle);
    BaseMat<N> rot;
    rot.Identity();
    rot[1][1] = c;
    rot[1][2] = s;
    rot[2][1] = -s;
    rot[2][2] = c;
    *this = *this * rot;
}

template<size_t N> void BaseMat<N>::RotateY(float angle) {
    if (N < 3) {
        return;
    }

    float s = sinf(angle);
    float c = cosf(angle);
    BaseMat<N> rot;
    rot.Identity();
    rot[0][0] = c;
    rot[0][2] = -s;
    rot[2][0] = s;
    rot[2][2] = c;
    *this = *this * rot;
}

template<size_t N> void BaseMat<N>::RotateZ(float angle) {
    float s = sinf(angle);
    float c = cosf(angle);
    BaseMat<N> rot;
    rot.Identity();
    rot[0][0] = c;
    rot[0][1] = s;
    rot[1][0] = -s;
    rot[1][1] = c;
    *this = *this * rot;
}

template<size_t N> void BaseMat<N>::Translate(float x, float y, float z) {
    BaseMat<N> M;
    M.Identity();
    if (N < 4) {
        M[2][0] = x;
        M[2][1] = y;
    } else {
        M[3][0] = x;
        M[3][1] = y;
        M[3][2] = z;
    }
    *this = *this * M;
}

template void BaseMat<4>::Translate(float, float, float);

template void BaseMat<4>::RotateX(float);
template void BaseMat<4>::RotateY(float);
template void BaseMat<4>::RotateZ(float);

void Mat4::Ortho(float l, float r, float b, float t, float n, float f) {
    auto M = memory;
	M[0][0] = 2.f/(r-l);
	M[0][1] = M[0][2] = M[0][3] = 0.f;

	M[1][1] = 2.f/(t-b);
	M[1][0] = M[1][2] = M[1][3] = 0.f;

	M[2][2] = -2.f/(f-n);
	M[2][0] = M[2][1] = M[2][3] = 0.f;

	M[3][0] = -(r+l)/(r-l);
	M[3][1] = -(t+b)/(t-b);
	M[3][2] = -(f+n)/(f-n);
	M[3][3] = 1.f;
}

Mat4 OrthoProjection(float l, float r, float b, float t, float n, float f) {
    Mat4 m;
    m.Ortho(l,r,b,t,n,f);
    return m;
}

void Mat4::Perspective(float y_fov, float aspect, float n, float f) {
    auto M = memory;
	float const a = 1.f / tanf(y_fov / 2.f);

	M[0][0] = a / aspect;
	M[0][1] = 0.f;
	M[0][2] = 0.f;
	M[0][3] = 0.f;

	M[1][0] = 0.f;
	M[1][1] = a;
	M[1][2] = 0.f;
	M[1][3] = 0.f;

	M[2][0] = 0.f;
	M[2][1] = 0.f;
	M[2][2] = -((f + n) / (f - n));
	M[2][3] = -1.f;

	M[3][0] = 0.f;
	M[3][1] = 0.f;
	M[3][2] = -((2.f * f * n) / (f - n));
	M[3][3] = 0.f;
}

Mat4 PerspectiveProjection(float y_fov, float aspect, float n, float f) {
    Mat4 m;
    m.Perspective(y_fov, aspect, n, f);
    return m;
}

template<size_t N> BaseMat<N> BaseMat<N>::operator *(const BaseMat<N> &b) const {
    BaseMat<N> temp;

    for (int c=0; c<N; c++) {
        for (int r=0; r<N; r++) {
            temp[c][r] = 0.f;
            for (int k=0; k<N; k++) {
                temp[c][r] += memory[k][r] * b[c][k];
            }
        }
    }

    return temp;
}

Mat4 LookAt(Vec3 const eye, Vec3 const center, Vec3 const up) {
    auto f = center - eye;
    f.Normalize();

    auto s = f.CrossProduct(up);
    s.Normalize();

    auto t = s.CrossProduct(f);

    Mat4 m;

	m[0][0] =  s[0];
	m[0][1] =  t[0];
	m[0][2] = -f[0];
	m[0][3] =   0.f;

	m[1][0] =  s[1];
	m[1][1] =  t[1];
	m[1][2] = -f[1];
	m[1][3] =   0.f;

	m[2][0] =  s[2];
	m[2][1] =  t[2];
	m[2][2] = -f[2];
	m[2][3] =   0.f;

	m[3][0] =  0.f;
	m[3][1] =  0.f;
	m[3][2] =  0.f;
	m[3][3] =  1.f;

    m.Translate(-eye[0], -eye[1], -eye[2]);
	return m;
}

template<size_t N> std::ostream& operator<< (std::ostream& stream, const BaseMat<N>& m) {
    stream << "Mat" << N << "(";
    for (int i = 0; i < N; i++) {
        stream << "[";
        for (int j = 0; j < N; j++) {
            stream << m[i][j];
            if (j != N-1) {
                stream << ",";
            }
        }
        stream << "]";
        if (i != N-1) {
            stream << ",\n     ";
        }
    }
    stream << ")";
    return stream;
}

template std::ostream& operator<< (std::ostream& stream, const BaseMat<2>& v);
template std::ostream& operator<< (std::ostream& stream, const BaseMat<3>& v);
template std::ostream& operator<< (std::ostream& stream, const BaseMat<4>& v);
