#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <iostream>
#include <cmath>
#include <vector>

extern const float PI;

template <class t> 
struct Vec4;


template <class t> 
struct Vec2 
{
	union {
		struct {t u, v;};
		struct {t x, y;};
		t raw[2];
	};
	Vec2() : u(0), v(0) {}
	Vec2(t _u, t _v) : u(_u),v(_v) {}
	inline Vec2<t> operator +(const Vec2<t> &V) const { return Vec2<t>(u+V.u, v+V.v); }
	inline Vec2<t> operator -(const Vec2<t> &V) const { return Vec2<t>(u-V.u, v-V.v); }
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(u*f, v*f); }
	inline t	   operator[](int i)      		const { return raw[i]; }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);
};

template <class t> 
struct Vec3 
{
	union {
		struct {t x, y, z;};
		struct { t ivert, iuv, inorm; };
		t raw[3];
	};
	Vec3() : x(0), y(0), z(0) {}
	Vec3(t _x) : x(_x), y(_x), z(_x) {}
	Vec3(Vec4<t> v) : x(v.x), y(v.y), z(v.z) {}
	Vec3(t _x, t _y, t _z) : x(_x),y(_y),z(_z) {}
	Vec3(Vec2<t> v, t _z) : x(v.x),y(v.y),z(_z) {}
	template <class u> Vec3<t>(const Vec3<u> &v);
	inline Vec3<t> operator ^(const Vec3<t> &v) const { return Vec3<t>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
	inline Vec3<t> operator +(const Vec3<t> &v) const { return Vec3<t>(x+v.x, y+v.y, z+v.z); }
	inline Vec3<t> operator -(const Vec3<t> &v) const { return Vec3<t>(x-v.x, y-v.y, z-v.z); }
	inline Vec3<t> operator *(float f)          const { return Vec3<t>(x*f, y*f, z*f); }
	inline t       operator *(const Vec3<t> &v) const { return x*v.x + y*v.y + z*v.z; }
	inline t	   operator[](const int i)        const { return raw[i]; }
	float norm () const { return std::sqrt(x*x+y*y+z*z); }
	Vec3<t> & normalize(t l=1) { *this = (*this)*(l/norm()); return *this; }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
};

template <class t> 
struct Vec4 
{
	union {
		struct {t x, y, z, w;};
		t raw[4];
	};
	Vec4() : x(0), y(0), z(0), w(1) {}
	Vec4(t _x, t _y, t _z, t _w) : x(_x),y(_y),z(_z), w(_w) {}
	Vec4(Vec3<t> v, t _w) : x(v.x),y(v.y),z(v.z), w(_w) {}
	Vec4(Vec3<t> v) : x(v.x),y(v.y),z(v.z), w(1) {}

	inline Vec4<t> operator +(const Vec4<t> &v) const { return Vec4<t>(x+v.x, y+v.y, z+v.z, w+v.w); }
	inline Vec4<t> operator -(const Vec4<t> &v) const { return Vec4<t>(x-v.x, y-v.y, z-v.z, w-v.w); }
	inline Vec4<t> operator *(float f)          const { return Vec4<t>(x*f, y*f, z*f, w*f); }
	inline t&	   operator[](const int i)       			  { return raw[i]; }
	Vec4<t> & NormalByW() { *this = (*this)*(1.0/(this->w)); return *this; }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec4<t>& v);
};


typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;
typedef Vec4<float> Vec4f;
typedef Vec4<int>   Vec4i;

template <> template <> Vec3<int>::Vec3(const Vec3<float> &v);
template <> template <> Vec3<float>::Vec3(const Vec3<int> &v);

template <class t> 
std::ostream& operator<<(std::ostream& s, Vec2<t>& v) 
{
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t> 
std::ostream& operator<<(std::ostream& s, Vec3<t>& v) 
{
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

template <class t> 
std::ostream& operator<<(std::ostream& s, Vec4<t>& v) 
{
	s << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")\n";
	return s;
}


//////////////////////////////////////////////////////////////////////////////////////////////

extern const int DEFAULT_ALLOC;

class Matrix {
    std::vector<std::vector<float> > m;
    int rows, cols;
public:
    Matrix(int r=DEFAULT_ALLOC, int c=DEFAULT_ALLOC);
    inline int nrows();
    inline int ncols();

    static Matrix identity(int dimensions);
    std::vector<float>& operator[](const int i);
    Matrix operator*(const Matrix& a);
	Vec4f operator*(Vec4f a);
    Matrix transpose();
    Matrix inverse();

    friend std::ostream& operator<<(std::ostream& s, Matrix& m);
};

/////////////////////////////////////////////////////////////////////////////////////////////

struct Transform
{
	Vec3f position;
	Vec3f scale;
	Vec3f rotation;

	Transform();
	Transform(Vec3f p);
	Transform(Vec3f p, Vec3f r);
	Transform(Vec3f p, Vec3f s, Vec3f r);
};

#endif