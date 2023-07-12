#pragma once

#include <math.h>
#include <cfloat>
#include "const.h"
#define M_RADPI		57.295779513082
#define PI			3.14159265358979323846
#define DEG2RAD(x)	((float)(x) * (float)((float)(PI) / 180.0f))
#define RAD2DEG(x)	((float)(x) * (float)(180.0f / (float)(PI)))

typedef float vec_t;
inline unsigned long& FloatBits( vec_t& f )
{
	return *reinterpret_cast< unsigned long* >( &f );
}

inline unsigned long const& FloatBits( vec_t const& f )
{
	return *reinterpret_cast< unsigned long const* >( &f );
}

inline vec_t BitsToFloat( unsigned long i )
{
	return *reinterpret_cast< vec_t* >( &i );
}

inline bool IsFinite( vec_t f )
{
	return ( ( FloatBits( f ) & 0x7F800000 ) != 0x7F800000 );
}

inline unsigned long FloatAbsBits( vec_t f )
{
	return FloatBits( f ) & 0x7FFFFFFF;
}

class vector_2d
{
public:
	float x, y;

	vector_2d(void)
	{
		x = y = 0.0f;
	}

	vector_2d(float X, float Y)
	{
		x = X; y = Y;
	}

	vector_2d(float* v)
	{
		x = v[0]; y = v[1];
	}

	vector_2d(const float* v)
	{
		x = v[0]; y = v[1];
	}

	vector_2d(const vector_2d& v)
	{
		x = v.x; y = v.y;
	}

	vector_2d& operator=(const vector_2d& v)
	{
		x = v.x; y = v.y; return *this;
	}

	float& operator[](int i)
	{
		return ((float*)this)[i];
	}

	float operator[](int i) const
	{
		return ((float*)this)[i];
	}

	vector_2d& operator+=(const vector_2d& v)
	{
		x += v.x; y += v.y; return *this;
	}

	vector_2d& operator-=(const vector_2d& v)
	{
		x -= v.x; y -= v.y; return *this;
	}

	vector_2d& operator*=(const vector_2d& v)
	{
		x *= v.x; y *= v.y; return *this;
	}

	vector_2d& operator/=(const vector_2d& v)
	{
		x /= v.x; y /= v.y; return *this;
	}

	vector_2d& operator+=(float v)
	{
		x += v; y += v; return *this;
	}

	vector_2d& operator-=(float v)
	{
		x -= v; y -= v; return *this;
	}

	vector_2d& operator*=(float v)
	{
		x *= v; y *= v; return *this;
	}

	vector_2d& operator/=(float v)
	{
		x /= v; y /= v; return *this;
	}

	vector_2d operator+(const vector_2d& v) const
	{
		return vector_2d(x + v.x, y + v.y);
	}

	vector_2d operator-(const vector_2d& v) const
	{
		return vector_2d(x - v.x, y - v.y);
	}

	vector_2d operator*(const vector_2d& v) const
	{
		return vector_2d(x * v.x, y * v.y);
	}

	vector_2d operator/(const vector_2d& v) const
	{
		return vector_2d(x / v.x, y / v.y);
	}

	vector_2d operator+(float v) const
	{
		return vector_2d(x + v, y + v);
	}

	vector_2d operator-(float v) const
	{
		return vector_2d(x - v, y - v);
	}

	vector_2d operator*(float v) const
	{
		return vector_2d(x * v, y * v);
	}

	vector_2d operator/(float v) const
	{
		return vector_2d(x / v, y / v);
	}

	bool is_valid( )
	{
		return IsFinite( x ) && IsFinite( y );
	}

	void set(float X = 0.0f, float Y = 0.0f)
	{
		x = X; y = Y;
	}

	float length(void) const
	{
		return sqrtf(x * x + y * y);
	}

	float length_sqr(void) const
	{
		return (x * x + y * y);
	}

	float dist_to(const vector_2d& v) const
	{
		return (*this - v).length();
	}

	float dist_to_sqr(const vector_2d& v) const
	{
		return (*this - v).length_sqr();
	}

	float dot(const vector_2d& v) const
	{
		return (x * v.x + y * v.y);
	}

	bool is_zero(void) const
	{
		return (x > -0.01f && x < 0.01f &&
				y > -0.01f && y < 0.01f);
	}
};

enum struct EWeaponType
{
	UNKNOWN, HITSCAN, PROJECTILE, MELEE
};

class vector
{
public:
	float x, y, z;

public:
	vector(void)
	{
		x = y = z = 0.0f;
	}

	void zero()
	{
		x = y = z = 0.f;
	}

	vector(float X, float Y, float Z)
	{
		x = X; y = Y; z = Z;
	}

	__forceinline void clear()
	{
		x = y = z = 0.f;
	}

	vector(float* v)
	{
		x = v[0]; y = v[1]; z = v[2];
	}

	vector(const float* v)
	{
		x = v[0]; y = v[1]; z = v[2];
	}

	vector(const vector& v)
	{
		x = v.x; y = v.y; z = v.z;
	}

	vector(const vector_2d& v)
	{
		x = v.x; y = v.y; z = 0.0f;
	}

	vector& operator=(const vector& v)
	{
		x = v.x; y = v.y; z = v.z; return *this;
	}

	vector& operator=(const vector_2d& v)
	{
		x = v.x; y = v.y; z = 0.0f; return *this;
	}

	float& operator[](int i)
	{
		return ((float*)this)[i];
	}

	float operator[](int i) const
	{
		return ((float*)this)[i];
	}

	vector& operator+=(const vector& v)
	{
		x += v.x; y += v.y; z += v.z; return *this;
	}

	vector& operator-=(const vector& v)
	{
		x -= v.x; y -= v.y; z -= v.z; return *this;
	}

	vector& operator*=(const vector& v)
	{
		x *= v.x; y *= v.y; z *= v.z; return *this;
	}

	vector& operator/=(const vector& v)
	{
		x /= v.x; y /= v.y; z /= v.z; return *this;
	}

	vector& operator+=(float v)
	{
		x += v; y += v; z += v; return *this;
	}

	vector& operator-=(float v)
	{
		x -= v; y -= v; z -= v; return *this;
	}

	vector& operator*=(float v)
	{
		x *= v; y *= v; z *= v; return *this;
	}

	vector& operator/=(float v)
	{
		x /= v; y /= v; z /= v; return *this;
	}

	bool operator<=(const vector other) const
	{
		return fabsf(x) <= other.x &&
			fabsf(y) <= other.y &&
			fabsf(z) <= other.z;
	}
	bool operator>=(const vector other) const
	{
		return fabsf(x) >= other.x ||
			fabsf(y) >= other.y ||
			fabsf(z) >= other.z;
	}

	vector operator+(const vector& v) const
	{
		return vector(x + v.x, y + v.y, z + v.z);
	}

	vector operator-(const vector& v) const
	{
		return vector(x - v.x, y - v.y, z - v.z);
	}

	vector operator*(const vector& v) const
	{
		return vector(x * v.x, y * v.y, z * v.z);
	}

	vector operator/(const vector& v) const
	{
		return vector(x / v.x, y / v.y, z / v.z);
	}

	vector operator+(float v) const
	{
		return vector(x + v, y + v, z + v);
	}

	vector operator-(float v) const
	{
		return vector(x - v, y - v, z - v);
	}

	vector operator*(float v) const
	{
		return vector(x * v, y * v, z * v);
	}

	vector operator/(float v) const
	{
		return vector(x / v, y / v, z / v);
	}

	bool operator==(const vector& v) const
	{
		return (this->x == v.x && this->y == v.y && this->z == v.z);
	}

	bool operator<=(const float other) const
	{
		return fabsf(x) <= other &&
			fabsf(y) <= other &&
			fabsf(z) <= other;
	}

	bool is_valid( )
	{
		return IsFinite( x ) && IsFinite( y ) && IsFinite( z );
	}

	void set(float X = 0.0f, float Y = 0.0f, float Z = 0.0f)
	{
		x = X; y = Y; z = Z;
	}

	vector vector_ma(float scale, const vector& direction)
	{
		vector dest;
		dest.x = this->x + scale * direction.x;
		dest.y = this->y + scale * direction.y;
		dest.z = this->z + scale * direction.z;
		return dest;
	}

	float length(void) const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	float length_sqr(void) const
	{
		return (x * x + y * y + z * z);
	}

	vector angle_vector() const;

	vector look(vector target) const
	{
		target -= *this;
		vector angles;
		if (target.y == 0.0f && target.x == 0.0f)
		{
			angles.x = (target.z > 0.0f) ? 270.0f : 90.0f;
			angles.y = 0.0f;
		}
		else
		{
			angles.x = static_cast<float>(atan2(-target.z, target.length_2d()) * 180.f / PI);
			angles.y = static_cast<float>(atan2(target.y, target.x) * 180.f / PI);
		}

		angles.z = 0.0f;
		return angles;
	}

	vector angle_to() const
	{
		vector angles;
		if (y == 0.0f && x == 0.0f)
		{
			angles.x = (z > 0.0f) ? 270.0f : 90.0f;
			angles.y = 0.0f;
		}
		else
		{
			angles.x = static_cast<float>(atan2(-z, length_2d()) * 180.f / PI);
			angles.y = static_cast<float>(atan2(y, x) * 180.f / PI);
		}

		angles.z = 0.0f;
		return angles;
	}

	vector normalized() const
	{
		const float ln = length();
		if (isnan(ln) || ln == 0.f)
			return vector();
		vector ret = *this;
		ret.x /= ln;
		ret.y /= ln;
		ret.z /= ln;
		return ret;
	};

	float normalize()
	{
		float fl_length = length();
		float fl_length_normal = 1.f / (FLT_EPSILON + fl_length);

		x = x * fl_length_normal;
		y = y * fl_length_normal;
		z = z * fl_length_normal;

		return fl_length;
	}

	vector to_angle() const noexcept
	{
		return vector{ RAD2DEG(atan2(-z, hypot(x, y))),
					 RAD2DEG(atan2(y, x)),
					 0.0f };
	}

	vector from_angle() const noexcept
	{
		return vector{ (float)(cos(DEG2RAD(x)) * cos(DEG2RAD(y))),
					 (float)(cos(DEG2RAD(x)) * sin(DEG2RAD(y))),
					 (float)( - sin(DEG2RAD(x)))};
	}

	void rotate_2d(const float& flYaw)
	{
		float s, c, r = DEG2RAD(flYaw);
		s = (float)sin(r); c = (float)cos(r);

		float _x = x, _y = y;

		x = (_x * c) - (_y * s);
		y = (_x * s) + (_y * c);
	}

	void cross_product(const vector& b, vector& result)
	{
		result.x = this->y * b.z - this->z * b.y;
		result.y = this->z * b.x - this->x * b.z;
		result.z = this->x * b.y - this->y * b.x;
	}

	float normalize_in_place()
	{
		return normalize();
	}

	float length_2d(void) const
	{
		return sqrtf(x * x + y * y);
	}

	float length_2d_sqr(void) const
	{
		return (x * x + y * y);
	}

	float dist_to(const vector& v) const
	{
		return (*this - v).length();
	}

	float dist_to_sqr(const vector& v) const
	{
		return (*this - v).length_sqr();
	}

	float dist_2d(const vector& v) const
	{
		return (*this - v).length_2d();
	}

	float dot(const vector& v) const
	{
		return (x * v.x + y * v.y + z * v.z);
	}

	vector cross(const vector& v) const
	{
		return vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	bool is_zero(void) const
	{
		return (x > -0.01f && x < 0.01f &&
				y > -0.01f && y < 0.01f &&
				z > -0.01f && z < 0.01f);
	}

	vector scale(float fl)
	{
		return vector(x * fl, y * fl, z * fl);
	}

	void init(float ix = 0, float iy = 0, float iz = 0)
	{
		x = ix; y = iy; z = iz;
	}

	void mult_add(vector& vOther, float fl)
	{
		x += fl * vOther.x;
		y += fl * vOther.y;
		z += fl * vOther.z;
	}

	vector cross_product(const vector& b)
	{
		return vector(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
	}
};

class vector_4d
{
public:
	float x, y, z, w;
};

struct matrix3x4_t
{
public:
	matrix3x4_t() {}
	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23)
	{
		matrix3x4[0][0] = m00;	matrix3x4[0][1] = m01; matrix3x4[0][2] = m02; matrix3x4[0][3] = m03;
		matrix3x4[1][0] = m10;	matrix3x4[1][1] = m11; matrix3x4[1][2] = m12; matrix3x4[1][3] = m13;
		matrix3x4[2][0] = m20;	matrix3x4[2][1] = m21; matrix3x4[2][2] = m22; matrix3x4[2][3] = m23;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	void init(const vector& xAxis, const vector& yAxis, const vector& zAxis, const vector& vecOrigin)
	{
		matrix3x4[0][0] = xAxis.x; matrix3x4[0][1] = yAxis.x; matrix3x4[0][2] = zAxis.x; matrix3x4[0][3] = vecOrigin.x;
		matrix3x4[1][0] = xAxis.y; matrix3x4[1][1] = yAxis.y; matrix3x4[1][2] = zAxis.y; matrix3x4[1][3] = vecOrigin.y;
		matrix3x4[2][0] = xAxis.z; matrix3x4[2][1] = yAxis.z; matrix3x4[2][2] = zAxis.z; matrix3x4[2][3] = vecOrigin.z;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	matrix3x4_t(const vector& xAxis, const vector& yAxis, const vector& zAxis, const vector& vecOrigin)
	{
		init(xAxis, yAxis, zAxis, vecOrigin);
	}

	inline void invalidate(void)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				matrix3x4[i][j] = 0x7FC00000;
			}
		}
	}

	float* operator[](int i) { return matrix3x4[i]; }
	const float* operator[](int i) const { return matrix3x4[i]; }
	float* base() { return &matrix3x4[0][0]; }
	const float* base() const { return &matrix3x4[0][0]; }

	float matrix3x4[3][4];
};

using QAngle = vector;
using RadianEuler = vector;
using VelocityImpulse = vector;
using AngularImpulse = vector;

inline float k_flMaxEntityPosCoord = MAX_COORD_FLOAT;
inline float k_flMaxEntityEulerAngle = 360.0 * 1000.0f;

inline bool IsEntityCoordinateReasonable( const vec_t c )
{
	float r = k_flMaxEntityPosCoord;
	return c > -r && c < r;
}

inline bool IsEntityQAngleReasonable( const QAngle& q )
{
	float r = k_flMaxEntityEulerAngle;
	return
		q.x > -r && q.x < r &&
		q.y > -r && q.y < r &&
		q.z > -r && q.z < r;
}